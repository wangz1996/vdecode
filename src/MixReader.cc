#include "WaveReader.hh"

int WaveReader::findMixHead(){
	char buffer[1];
	while(file->rdbuf()->sgetn(buffer,sizeof(buffer))>0){
		unsigned char byte = static_cast<unsigned char>(buffer[0]);
		if(byte == 0x55){ //Scientific data head return 0
			char buffer2[3];
			file->rdbuf()->sgetn(buffer2,sizeof(buffer2));
			unsigned char b1 = static_cast<unsigned char>(buffer2[0]);
			unsigned char b2 = static_cast<unsigned char>(buffer2[1]);
			unsigned char b3 = static_cast<unsigned char>(buffer2[2]);
			if(b1 == 0xaa && b2 == 0xeb && b3 == 0x90){
				nhead++;
				return 0;
			}
			else{
				file->seekg(-3,std::ios::cur); // Not a scientific data head, seek back
				return -1;
			}
		}
		else if(byte == 0xeb){
			char buffer2[2];
			file->rdbuf()->sgetn(buffer2,sizeof(buffer2));
			unsigned char b1 = static_cast<unsigned char>(buffer2[0]); // Should be 90
			unsigned char b2 = static_cast<unsigned char>(buffer2[1]); // Should be xB""
			if(b1 == 0x90){
				if((b2&0x0F) == 0x0B){
					unsigned char x = (unsigned char)((b2>>4) & 0x0F);
					MixFEEID = int(x)-4;
					if(MixFEEID>=1 && MixFEEID<=4){
						return 1;
					}
					else{
						std::cout<<"Not 1-4 return false"<<std::endl;
						file->seekg(-2,std::ios::cur);
						return -1;
					}
				}
			}
			else{
				file->seekg(-2,std::ios::cur); // Not a temperature data head, seek back
				return -1;
			}
		}
	}
	return -2;
}

void WaveReader::decodeMix(const std::string& filename){
    getOutputName(std::string("result_"),filename);
    fout = new TFile(oname.c_str(),"RECREATE");
    tout = new TTree("tree","tree");
	tout->Branch("EventID",&EventID);
	tout->Branch("CellID",&CellID);
	tout->Branch("CellADC",&CellADC);
	tout->Branch("CellPLAT",&CellPLAT);
	tout->Branch("Temp",&MixTemperature);
	tout->Branch("Time",&MixTime);
	tout->Branch("Current",&MixCurrent);
    openFile(filename);
    while(!file->eof()){
		int HeadClass=findMixHead();
		if(HeadClass==-1){
			continue;
		}
		else if(HeadClass==0){
			if(readAmp())npackages+=0.25;
		}
		else if(HeadClass==1){
			readMixTemp();
		}
		else{
			break;
		}
    }
	std::cout<<"Total head: "<<nhead<<std::endl;
	std::cout<<"Total packages: "<<npackages<<std::endl;
    fout->cd();
    tout->Write();
    fout->Close();
}

bool WaveReader::readMixTemp(){
	if(MixFEEID!=1){
		return true;
	}
	MixTime++;
	//124 Bytes/Package
	//Start to read
	char rest[14];
	file->rdbuf()->sgetn(rest,sizeof(rest));
	auto f_current = [rest](const int& i){
		int index = i*2;
		float cif=-1.;
		auto ci0 = static_cast<unsigned char>(rest[index]);
		auto ci1 = static_cast<unsigned char>(rest[index+1]);
		unsigned short ci = (static_cast<unsigned short>(ci0 & 0x0F) << 8) | ci1;
		if(i==0){
			cif = (ci*2500./4096.-245.1)/20./0.01;
		}
		else{
			cif = (ci*2500./4096.-245.1)/20./0.25;
		}
		return cif;
	};
	MixCurrent = f_current(0);
	//Temperature
	auto f_temperature = [rest](const int& i){
		int index = 6 + i*2;
		float tif=-1.;
		auto ti0 = static_cast<unsigned char>(rest[index]);
		auto ti1 = static_cast<unsigned char>(rest[index+1]);
		unsigned short ti = (static_cast<unsigned short>(ti0 & 0x0F) << 8) | ti1;
		float fx = 10.*(ti*2500./4096.*2000./2100.)/(2500.-(ti*2500./4096.*2000./2100.));
		tif = (17.-fx)/(12./25.);
		return tif;
	};
	MixTemperature = f_temperature(0);
	MixTempEventCount++;
	return false;
}
