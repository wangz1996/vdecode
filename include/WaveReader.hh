#ifndef WAVEREADER_HH
#define WAVEREADER_HH
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <set>
#include <bitset>
#include <unordered_map>
#include "TH1D.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"

enum class WorkMode{
    WAVE,
    AMP,
    TEMP,
    MIX
};

enum class DetType{
    CALO,
    ITK
};
// class CTData {
// public:
//   std::vector<std::vector<float>> ct_array;

//   CTData() {}
// };

struct PairHash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        
        // 简单的哈希组合方法
        return h1 ^ h2;
        
        // 更好的哈希组合方法（来自Boost）
        // return h1 ^ (h2 << 1);
    }
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
    void decodeData(const std::string& filename);
    void decodeTemp(const std::string& filename);
    void decodeMix(const std::string& filename);
    void setMode(const std::string& mode);
private:
    // 私有构造函数，防止外部实例化
    WaveReader()=default;
    ~WaveReader();
    //Find head
    bool findHead();
    bool findTempHead();
    int findMixHead(); // return 0 for scientific, 1 for temperature
    bool readWave();
    bool readAmp();
    bool readTemp();
    bool readMixTemp();
    void ampClear();
    //Read file
    bool openFile(const std::string& filename);
    //Get output name
    void getOutputName(const std::string& prefix,const std::string& filename);
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
    DetType dettype;
    float npackages=0.;
    int nhead=0;
    int nevents_skipped=0;
    std::vector<int> CellID; // CryID-FEEID-MBID-GID-CHNID
    std::vector<int> CellADC;
    std::vector<int> CellPLAT;
    float MixTemperature; // Temperature for MixReader
    float MixCurrent; // Current for MixReader
    int EventID=0;
    int EventCount=0;
    int MixTempEventCount=0;
    int MixTime;
    std::set<int> set_FEEID;

    //Temperature Current Data
    int TPointID=0;
    static constexpr int NFEE = 4;
    int FEEID=-1;
    int MixFEEID=-1;
    // float C[4][3];
    // float T[4][4];
    std::vector<float> C0;
    std::vector<float> C1;
    std::vector<float> C2;
    std::vector<float> T0;
    std::vector<float> T1;
    std::vector<float> T2;
    std::vector<float> T3;
    // std::vector<std::vector<float>> T;
    // std::vector<std::vector<float>> C;
    // CTData T;
    // CTData C;
    //Clear function
    void clear();

    //Constants
    static constexpr double e=1.602176634e-19;
    static constexpr double lowgain=0.549; //ADC/fC
    static constexpr double highgain=5.614; //ADC/fC
    static constexpr double MIP_Edep=36.; //MeV
    static constexpr double gain=50.;

    std::unordered_map<std::pair<int, int>, std::pair<int, int>, PairHash> channelMap = { //<FEEID,ChnID> <GID,CrystalID>
    {{3, 22}, {1, 1}}, {{3, 25}, {0, 1}}, {{4, 22}, {1, 1}}, {{4, 25}, {0, 1}},
    {{3, 14}, {1, 2}}, {{3, 21}, {0, 2}}, {{4, 14}, {1, 2}}, {{4, 21}, {0, 2}},
    {{1, 4}, {1, 3}},  {{1, 17}, {0, 3}}, {{2, 4}, {1, 3}},  {{2, 17}, {0, 3}},
    {{1, 2}, {1, 4}},  {{1, 9}, {0, 4}},  {{2, 2}, {1, 4}},  {{2, 9}, {0, 4}},
    {{1, 0}, {1, 5}},  {{1, 1}, {0, 5}},  {{2, 0}, {1, 5}},  {{2, 1}, {0, 5}},
    {{3, 20}, {1, 6}}, {{3, 23}, {0, 6}}, {{4, 20}, {1, 6}}, {{4, 23}, {0, 6}},
    {{3, 12}, {1, 7}}, {{3, 19}, {0, 7}}, {{4, 12}, {1, 7}}, {{4, 19}, {0, 7}},
    {{3, 6}, {1, 8}},  {{3, 17}, {0, 8}}, {{4, 6}, {1, 8}},  {{4, 17}, {0, 8}},
    {{1, 8}, {1, 9}},  {{1, 11}, {0, 9}}, {{2, 8}, {1, 9}},  {{2, 11}, {0, 9}},
    {{1, 6}, {1, 10}}, {{1, 3}, {0, 10}}, {{2, 6}, {1, 10}}, {{2, 3}, {0, 10}},
    {{3, 18}, {1, 11}},{{3, 7}, {0, 11}}, {{4, 18}, {1, 11}},{{4, 7}, {0, 11}},
    {{3, 10}, {1, 12}},{{3, 13}, {0, 12}},{{4, 10}, {1, 12}},{{4, 13}, {0, 12}},
    {{1, 10}, {1, 13}},{{1, 19}, {0, 13}},{{2, 10}, {1, 13}},{{2, 19}, {0, 13}},
    {{1, 14}, {1, 14}},{{1, 13}, {0, 14}},{{2, 14}, {1, 14}},{{2, 13}, {0, 14}},
    {{1, 20}, {1, 15}},{{1, 5}, {0, 15}}, {{2, 20}, {1, 15}},{{2, 5}, {0, 15}},
    {{3, 16}, {1, 16}},{{3, 5}, {0, 16}}, {{4, 16}, {1, 16}},{{4, 5}, {0, 16}},
    {{3, 8}, {1, 17}}, {{3, 11}, {0, 17}},{{4, 8}, {1, 17}}, {{4, 11}, {0, 17}},
    {{3, 4}, {1, 18}}, {{3, 15}, {0, 18}},{{4, 4}, {1, 18}}, {{4, 15}, {0, 18}},
    {{1, 16}, {1, 19}},{{1, 15}, {0, 19}},{{2, 16}, {1, 19}},{{2, 15}, {0, 19}},
    {{1, 22}, {1, 20}},{{1, 7}, {0, 20}}, {{2, 22}, {1, 20}},{{2, 7}, {0, 20}},
    {{3, 2}, {1, 21}}, {{3, 3}, {0, 21}}, {{4, 2}, {1, 21}}, {{4, 3}, {0, 21}},
    {{3, 0}, {1, 22}}, {{3, 9}, {0, 22}}, {{4, 0}, {1, 22}}, {{4, 9}, {0, 22}},
    {{1, 12}, {1, 23}},{{1, 25}, {0, 23}},{{2, 12}, {1, 23}},{{2, 25}, {0, 23}},
    {{1, 18}, {1, 24}},{{1, 23}, {0, 24}},{{2, 18}, {1, 24}},{{2, 23}, {0, 24}},
    {{1, 24}, {1, 25}},{{1, 21}, {0, 25}},{{2, 24}, {1, 25}},{{2, 21}, {0, 25}}
};

};

#endif
