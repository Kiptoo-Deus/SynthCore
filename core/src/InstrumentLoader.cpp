#include "synthcore/InstrumentLoader.hpp"
#include "synthcore/WavLoader.hpp"
#include <fstream>
#include <sstream>

namespace synthcore {

bool InstrumentLoader::loadFromDirectory(const std::string& dir, 
                                        const std::string& mapFile, 
                                        SampleMap& outMap) {
    std::string mapPath = combinePath(dir, mapFile);
    std::ifstream file(mapPath);
    if (!file.is_open()) {
        return false;
    }

    outMap.clear();
    std::string line;
    int lineNum = 0;

    while (std::getline(file, line)) {
        lineNum++;
        
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::string filename;
        uint8_t rootNote, loNote, hiNote, loVel, hiVel;
        int rrGroup;

        if (!parseMappingLine(line, filename, rootNote, loNote, hiNote, loVel, hiVel, rrGroup)) {
            continue;
        }

        std::string wavPath = combinePath(dir, filename);
        auto loadResult = WavLoader::load(wavPath);

        if (!loadResult.success) {
            continue;
        }

        SampleRegion region;
        region.samples = std::move(loadResult.samples);
        region.sampleRate = loadResult.sampleRate;
        region.rootNote = rootNote;
        region.loNote = loNote;
        region.hiNote = hiNote;
        region.loVel = loVel;
        region.hiVel = hiVel;
        region.loopStart = loadResult.loopStart;
        region.loopEnd = loadResult.loopEnd;
        region.hasLoop = loadResult.hasLoop;
        region.rrGroup = rrGroup;

        if (region.hasLoop) {
            region.loopType = LoopType::Sustain;
        } else {
            region.loopType = LoopType::None;
        }

        outMap.addRegion(std::move(region));
    }

    return outMap.getRegionCount() > 0;
}

std::string InstrumentLoader::combinePath(const std::string& dir, const std::string& filename) {
    if (dir.empty()) return filename;
    
    char lastChar = dir[dir.length() - 1];
    if (lastChar == '/' || lastChar == '\\') {
        return dir + filename;
    }
    
    return dir + "/" + filename;
}

bool InstrumentLoader::parseMappingLine(const std::string& line, 
                                       std::string& filename,
                                       uint8_t& rootNote,
                                       uint8_t& loNote,
                                       uint8_t& hiNote,
                                       uint8_t& loVel,
                                       uint8_t& hiVel,
                                       int& rrGroup) {
    std::istringstream iss(line);
    int root, lo, hi, loV, hiV, rr = 0;

    if (!(iss >> filename >> root >> lo >> hi >> loV >> hiV)) {
        return false;
    }

    iss >> rr;

    if (root < 0 || root > 127 || lo < 0 || lo > 127 || 
        hi < 0 || hi > 127 || loV < 0 || loV > 127 || hiV < 0 || hiV > 127) {
        return false;
    }

    rootNote = static_cast<uint8_t>(root);
    loNote = static_cast<uint8_t>(lo);
    hiNote = static_cast<uint8_t>(hi);
    loVel = static_cast<uint8_t>(loV);
    hiVel = static_cast<uint8_t>(hiV);
    rrGroup = rr;

    return true;
}

} // namespace synthcore
