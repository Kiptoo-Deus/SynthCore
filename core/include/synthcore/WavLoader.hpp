#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace synthcore {

struct LoadResult {
    std::vector<float> samples;
    int sampleRate;
    int numChannels;
    int bitDepth;
    uint32_t loopStart;
    uint32_t loopEnd;
    bool hasLoop;
    bool success;
    std::string errorMessage;
};

class WavLoader {
public:
    static LoadResult load(const std::string& filePath);

private:
    static bool readRiffHeader(FILE* file, uint32_t& fileSize);
    static bool readFmtChunk(FILE* file, int& sampleRate, int& numChannels, int& bitDepth);
    static bool readDataChunk(FILE* file, std::vector<float>& samples, int numChannels, int bitDepth);
    static bool readSmplChunk(FILE* file, uint32_t chunkSize, uint32_t& loopStart, uint32_t& loopEnd, bool& hasLoop);
    static int32_t read24BitSample(const uint8_t* bytes);
};

} // namespace synthcore
