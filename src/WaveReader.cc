#include "WaveReader.hh"

bool WaveReader::openFile(const std::string& filename){
    file = new std::ifstream(filename,std::ios::binary);
    if (!file->is_open()) {
        std::cerr << "Can not open file: "<<filename << std::endl;
        return false;
    }
    return true;
}

bool WaveReader::findHead(){
	char buffer[1];
	while(file->rdbuf()->sgetn(buffer,sizeof(buffer))>0){
		unsigned char byte = static_cast<unsigned char>(buffer[0]);
		if(byte == 0x55){
			char buffer2[3];
			file->rdbuf()->sgetn(buffer2,sizeof(buffer2));
			unsigned char b1 = static_cast<unsigned char>(buffer2[0]);
			unsigned char b2 = static_cast<unsigned char>(buffer2[1]);
			unsigned char b3 = static_cast<unsigned char>(buffer2[2]);
			if(b1 == 0xaa && b2 == 0xeb && b3 == 0x90){
				return true;
			}
		}
	}
	return false;
}

bool WaveReader::readWave(){
	char btime[10];
	file->rdbuf()->sgetn(btime,sizeof(btime));
	char rest[6662];
	file->rdbuf()->sgetn(rest,sizeof(rest));
	unsigned char b1 = static_cast<unsigned char>(rest[6660]);
	unsigned char b2 = static_cast<unsigned char>(rest[6661]);
	if(b1==0x5a && b2==0xa5){
		for(size_t chn_i=0;chn_i<26;chn_i++){
			TString name = TString::Format("chn%d",chn_i+1);
			for(size_t byte_i=0;byte_i<128;byte_i++){
				int init_pos = 256*(chn_i) + byte_i*2;
				auto d1 = static_cast<unsigned char>(rest[init_pos]);
				auto d2 = static_cast<unsigned char>(rest[init_pos+1]);
				unsigned short fullvalue = (static_cast<unsigned short>(d1)<<8) | d2;
				chn[chn_i] = chn[chn_i] > fullvalue ? chn[chn_i]:fullvalue;
				if(byte_i<16){
                    chnplat[chn_i] += fullvalue;
                }
			}
            chnplat[chn_i] /= 16.;
            chn[chn_i] -= chnplat[chn_i];
		}
		tout->Fill();
		return true;
	}
	return false;
}

bool WaveReader::readAmp(){
	char btime[10];
	file->rdbuf()->sgetn(btime,sizeof(btime));
	char rest[110];
	file->rdbuf()->sgetn(rest,sizeof(rest));
	unsigned char b1 = static_cast<unsigned char>(rest[108]);
	unsigned char b2 = static_cast<unsigned char>(rest[109]);
	if(b1==0x5a && b2==0xa5){
		for(size_t chn_i=0;chn_i<26;chn_i++){
			auto d1 = static_cast<unsigned char>(rest[chn_i*4]);
			auto d2 = static_cast<unsigned char>(rest[chn_i*4+1]);
			auto d3 = static_cast<unsigned char>(rest[chn_i*4+2]);
			auto d4 = static_cast<unsigned char>(rest[chn_i*4+3]);
			unsigned short plat = (static_cast<unsigned short>(d1)<<8) | d2;
			unsigned short maxi = (static_cast<unsigned short>(d3)<<8) | d4;
            chnplat[chn_i] = plat;
            chn[chn_i] = maxi - plat;
		}
		tout->Fill();
		return true;
	}
	return false;
}

void WaveReader::decode(const std::string& filename){
    getOutputName(filename);
    fout = new TFile(oname.c_str(),"RECREATE");
    tout = new TTree("tree","tree");
    for(int i=0;i<NChn;i++){
        tout->Branch(TString::Format("chn%d",i+1),&chn[i]);
        tout->Branch(TString::Format("chnplat%d",i+1),&chnplat[i]);
    }
    openFile(filename);
    while(!file->eof()){
        if(findHead()){
            if(mode == WorkMode::WAVE){
                readWave();
            }
            else if(mode == WorkMode::AMP){
                readAmp();
            }
            else{
                continue;
            }
        }
        else{break;}
    }
    fout->cd();
    tout->Write();
    fout->Close();
}

void WaveReader::setMode(const std::string& mode){
    if(mode == "wave"){
        this->mode = WorkMode::WAVE;
    }
    else if(mode == "amp"){
        this->mode = WorkMode::AMP;
    }
    else{
        std::cerr<<"Unknown mode: "<<mode<<std::endl;
    }
}

void WaveReader::getOutputName(const std::string& filename){
    oname = filename.substr(filename.find_last_of("/")+1);
    oname = oname.substr(0,oname.find_last_of("."));
    oname = "result_"+oname+".root";
}

WaveReader::~WaveReader(){
    file->close();
    delete file;
    
}