#include "WaveReader.hh"

int main(int argc,char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <mode> <filename>" << std::endl;
        return 1;
    }
    WaveReader::getInstance().setMode(std::string(argv[1]));
    WaveReader::getInstance().decode(std::string(argv[2]));
    return 1;
}