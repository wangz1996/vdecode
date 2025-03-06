#ifndef WAVEREADER_HH
#define WAVEREADER_HH
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include "TH1D.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"

enum class WorkMode{
    WAVE,
    AMP
};
class WaveReader{
public:
static WaveReader& getInstance() {
        // C++11 起，局部静态变量在首次调用时初始化，并自动保证线程安全
        static WaveReader instance;
        return instance;
    }
    WaveReader(const WaveReader&) = delete;
    WaveReader& operator=(const WaveReader&) = delete;
    //Decode
    void decode(const std::string& filename);
    void setMode(const std::string& mode);
private:
    // 私有构造函数，防止外部实例化
    WaveReader()=default;
    ~WaveReader();
    //Find head
    bool findHead();
    bool readWave();
    bool readAmp();
    //Read file
    bool openFile(const std::string& filename);
    //Get output name
    void getOutputName(const std::string& filename);
    //Input file and output TFile and TTree
    std::ifstream *file;
    TFile *fout;
    TTree *tout;
    std::string oname; //output file name

    //Data
    static constexpr int NChn=26;
    float chn[NChn];
    float chnplat[NChn];
    WorkMode mode;
};

#endif