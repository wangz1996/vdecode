#include "WaveReader.hh"

bool WaveReader::openFile(const std::string& filename){
	std::string filename_last = filename;
	filename_last = filename_last.substr(filename_last.find_last_of("/")+1);
	if(filename_last.find("Calo")!=std::string::npos){
		std::cout<<"Calo"<<std::endl;
		dettype = DetType::CALO;
	}
	else if(filename_last.find("CsITK")!=std::string::npos){
		dettype = DetType::ITK;
	}
	else{
		dettype = DetType::CALO;
	}
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
				nhead++;
				return true;
			}
			else{
				file->seekg(-3,std::ios::cur); // Not a scientific data head, seek back
			}
		}
	}
	return false;
}

bool WaveReader::readWave(){
	if(EventCount==0){
		CellID = std::vector<int>(0, 0);
		CellADC = std::vector<int>(0, 0);
		CellPLAT = std::vector<int>(0, 0);
	}
	char btime[10];
	file->rdbuf()->sgetn(btime,sizeof(btime));
	unsigned char b7 = static_cast<unsigned char>(btime[7]);
	unsigned int FEEID = static_cast<int>((b7 & 0xF0) >> 4); //FEEID
	FEEID = FEEID > 4 ? FEEID - 4 : FEEID;
	char rest[6662];
	file->rdbuf()->sgetn(rest,sizeof(rest));
	unsigned char b1 = static_cast<unsigned char>(rest[6660]);
	unsigned char b2 = static_cast<unsigned char>(rest[6661]);
	if(b1==0x5a && b2==0xa5){
		for(size_t chn_i=0;chn_i<NChn;chn_i++){
			TString name = TString::Format("chn%d",chn_i+1);
			float plat =0.;
			float maxi=0.;
			for(size_t byte_i=0;byte_i<128;byte_i++){
				int init_pos = 256*(chn_i) + byte_i*2;
				auto d1 = static_cast<unsigned char>(rest[init_pos]);
				auto d2 = static_cast<unsigned char>(rest[init_pos+1]);
				unsigned short fullvalue = (static_cast<unsigned short>(d1)<<8) | d2;
				
				// chn[chn_i] = chn[chn_i] > fullvalue ? chn[chn_i]:fullvalue;
				maxi = maxi > fullvalue ? maxi:fullvalue;
				if(byte_i<16){
                    // chnplat[chn_i] += fullvalue;
					plat+=fullvalue;
                }

			}
            plat /= 16.;
			std::pair<int,int> gid_cryid = std::pair<int,int>(0,0);
			if(channelMap.count(std::pair<int,int>(FEEID,chn_i))==0){
				// std::cerr<<"No channel map for FEEID: "<<FEEID<<" chn: "<<chn_i<<std::endl;
				continue;
			}
			gid_cryid = channelMap[std::pair<int,int>(FEEID,chn_i)];
			// if(gid_cryid.second ==0 ){
			// 	std::cerr<<"No channel map for FEEID: "<<FEEID<<" chn: "<<chn_i<<std::endl;
			// }
			gid_cryid = channelMap[std::pair<int,int>(FEEID,chn_i)];
			int tmp_cellid = 100000 * gid_cryid.second + 10000 * FEEID + 1000 * ( FEEID % 2 ) + 100 * (gid_cryid.first) + chn_i;
			CellID.emplace_back(tmp_cellid);
			CellADC.emplace_back(static_cast<int>(maxi));
            CellPLAT.emplace_back(plat);
		}
		if(EventCount==3){
			tout->Fill();
			EventID++;
			EventCount=0;
		}
		else{
			EventCount++;
		}
		return true;
	}
	return false;
}

bool WaveReader::readAmp(){
	//124 Bytes/Package
	if(EventCount==0){
		CellID = std::vector<int>(0, 0);
		CellADC = std::vector<int>(0, 0);
		CellPLAT = std::vector<int>(0, 0);
	}

	char btime[10];
	file->rdbuf()->sgetn(btime,sizeof(btime));
	unsigned char b7 = static_cast<unsigned char>(btime[7]);
	unsigned int FEEID = static_cast<int>((b7 & 0xF0) >> 4); //FEEID
	FEEID = FEEID > 4 ? FEEID - 4 : FEEID;
	char rest[110];
	file->rdbuf()->sgetn(rest,sizeof(rest));
	// unsigned char trigger_id = static_cast<unsigned char>(btime[104]);
	// unsigned char trigger_id2 = static_cast<unsigned char>(btime[105]);
	// //concat lower part of id with all id2
	// unsigned int triggerid = static_cast<int>((trigger_id & 0x0F) << 8 | trigger_id2);
	// //Print trigger id in binary format
	// std::cout<<"triggerid: "<<triggerid<<" "<<std::bitset<16>(triggerid)<<std::endl;
	unsigned char b1 = static_cast<unsigned char>(rest[108]);
	unsigned char b2 = static_cast<unsigned char>(rest[109]);
	if(b1==0x5a && b2==0xa5){
		for(size_t chn_i=0;chn_i<NChn;chn_i++){
			auto d1 = static_cast<unsigned char>(rest[chn_i*4]);
			auto d2 = static_cast<unsigned char>(rest[chn_i*4+1]);
			auto d3 = static_cast<unsigned char>(rest[chn_i*4+2]);
			auto d4 = static_cast<unsigned char>(rest[chn_i*4+3]);
			unsigned short plat = (static_cast<unsigned short>(d1)<<8) | d2;
			unsigned short maxi = (static_cast<unsigned short>(d3)<<8) | d4;
			std::pair<int,int> gid_cryid = std::pair<int,int>(0,0);
			if(channelMap.count(std::pair<int,int>(FEEID,chn_i))==0){
				if(FEEID==3 || FEEID==4){
					if(chn_i==1 || chn_i==24){
						continue;
					}
				}
			}
			gid_cryid = channelMap[std::pair<int,int>(FEEID,chn_i)];
			if(gid_cryid.second ==0 ){
				// std::cerr<<"No channel map for FEEID: "<<FEEID<<" chn: "<<chn_i<<std::endl;
			}
			int tmp_cellid = gid_cryid.second * 100000 + 10000 * FEEID + 1000 * ( FEEID % 2 ) + 100 * (gid_cryid.first) + chn_i;
			CellID.emplace_back(tmp_cellid);
			CellADC.emplace_back(static_cast<int>(maxi));
            CellPLAT.emplace_back(plat);
		}
		if(dettype == DetType::ITK){
			tout->Fill();
			EventID++;
		} else if (dettype == DetType::CALO){
			if(EventCount==3){
				tout->Fill();
				EventID++;
				EventCount=0;
			}
			else{
				EventCount++;
			}
		}
		else{
			std::cerr<<"Unknown detector type"<<std::endl;
		}
		return true;
	}
	else{
		file->seekg(-120, std::ios_base::cur);
	}
	return false;
}
void WaveReader::decode(const std::string& filename){
	if(mode == WorkMode::TEMP){this->decodeTemp(filename);}
	else if(mode == WorkMode::MIX){this->decodeMix(filename);}
	else {this->decodeData(filename);}
}

void WaveReader::decodeTemp(const std::string& filename){
	getOutputName(std::string("temp_"),filename);
	fout = new TFile(oname.c_str(),"RECREATE");
	tout = new TTree("tree","tree");
	tout->Branch("TPointID",&TPointID);
	tout->Branch("C0",&C0);
	tout->Branch("C1",&C1);
	tout->Branch("C2",&C2);
	tout->Branch("T0",&T0);
	tout->Branch("T1",&T1);
	tout->Branch("T2",&T2);
	tout->Branch("T3",&T3);
	openFile(filename);
	auto total_size = file->seekg(0, std::ios::end).tellg();
	file->seekg(0, std::ios::beg);
	while(!file->eof()){
		auto current_pointer = file->tellg();
		if(findTempHead()){
			readTemp();
			npackages+=0.25;
		}
		else{
			std::cout<<"Return false from readTemp()"<<std::endl;
			if(current_pointer+120 > total_size){
				std::cout<<"Reached end of file"<<std::endl;
				break;
			}
			continue;
		}
	}
	std::cout<<"Total packages: "<<npackages<<std::endl;
	fout->cd();
	tout->Write();
	fout->Close();
}

void WaveReader::decodeData(const std::string& filename){
    getOutputName(std::string("result_"),filename);
    fout = new TFile(oname.c_str(),"RECREATE");
    tout = new TTree("tree","tree");
	tout->Branch("EventID",&EventID);
	tout->Branch("CellID",&CellID);
	tout->Branch("CellADC",&CellADC);
	tout->Branch("CellPLAT",&CellPLAT);
    openFile(filename);
    while(!file->eof()){
        if(findHead()){
            if(mode == WorkMode::WAVE){
                if(readWave())npackages+=0.25;
            }
            else if(mode == WorkMode::AMP){
                if(readAmp()){
					if(dettype == DetType::ITK){
						npackages+=1;
					}
					else if(dettype == DetType::CALO){
						npackages+=0.25;
					}
				}
            }
            else{
                continue;
            }
        }
        else{break;}
    }
	std::cout<<"Total head: "<<nhead<<std::endl;
	std::cout<<"Total packages: "<<npackages<<std::endl;
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
	else if(mode == "temp"){
		this->mode = WorkMode::TEMP;
	}
	else if(mode == "mix"){
		this->mode = WorkMode::MIX;
	}
    else{
        std::cerr<<"Unknown mode: "<<mode<<std::endl;
    }
}

void WaveReader::getOutputName(const std::string& prefix,const std::string& filename){
    oname = filename.substr(filename.find_last_of("/")+1);
    oname = oname.substr(0,oname.find_last_of("."));
    oname = prefix+oname+".root";
}

WaveReader::~WaveReader(){
    file->close();
    delete file;
    
}
