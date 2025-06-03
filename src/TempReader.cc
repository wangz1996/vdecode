#include "WaveReader.hh"

bool WaveReader::findTempHead(){
	char buffer[1];
	while(file->rdbuf()->sgetn(buffer,sizeof(buffer))>0){
		unsigned char byte = static_cast<unsigned char>(buffer[0]);
		if(byte == 0xeb){
			char buffer2[2];
			file->rdbuf()->sgetn(buffer2,sizeof(buffer2));
			unsigned char b1 = static_cast<unsigned char>(buffer2[0]); // Should be 90
			unsigned char b2 = static_cast<unsigned char>(buffer2[1]); // Should be xB""
			if(b1 == 0x90){
				if((b2&0x0F) == 0x0B){
					unsigned char x = (unsigned char)((b2>>4) & 0x0F);
					FEEID = int(x)-4;
					if(FEEID>=1 && FEEID<=4){
						return true;
					}
					else{
						std::cout<<"Not 1-4 return false"<<std::endl;
						file->seekg(-2,std::ios::cur);
						return false;
					}
				}
			}
		}
	}
	return false;
}

void WaveReader::clear(){
	C0=std::vector<float>(4,0.);
	C1=std::vector<float>(4,0.);
	C2=std::vector<float>(4,0.);
	T0=std::vector<float>(4,0.);
	T1=std::vector<float>(4,0.);
	T2=std::vector<float>(4,0.);
	T3=std::vector<float>(4,0.);
}

bool WaveReader::readTemp(){
	//124 Bytes/Package
	if(EventCount==0){
		TPointID++;
		clear();
	}
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
	C0[FEEID-1] = f_current(0);
	C1[FEEID-1] = f_current(1);
	C2[FEEID-1] = f_current(2);
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
	T0[FEEID-1] = f_temperature(0);
	T1[FEEID-1] = f_temperature(1);
	T2[FEEID-1] = f_temperature(2);
	T3[FEEID-1] = f_temperature(3);
	EventCount++;
	if(FEEID==4){
		if(EventCount==4){
			tout->Fill();
		}
		EventCount=0;
	}
	return false;
}
