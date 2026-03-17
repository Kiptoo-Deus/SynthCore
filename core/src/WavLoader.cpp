#include "synthcore/WavLoader.hpp"
#include <cstdio>
#include <cstring>
#include <algorithm>

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

namespace synthcore {

LoadResult WavLoader::load(const std::string& filePath) {
    LoadResult result{};
    result.success = false;
    result.hasLoop = false;
    result.loopStart = 0;
    result.loopEnd = 0;

    FILE* file = fopen(filePath.c_str(), "rb");
    if (!file) {
        result.errorMessage = "Failed to open file";
        return result;
    }

    uint32_t fileSize;
    if (!readRiffHeader(file, fileSize)) {
        result.errorMessage = "Invalid RIFF header";
        fclose(file);
        return result;
    }

    bool foundFmt = false;
    bool foundData = false;

    while (ftell(file) < static_cast<long>(fileSize + 8)) {
        char chunkId[5] = {0};
        if (fread(chunkId, 1, 4, file) != 4) break;

        uint32_t chunkSize;
        if (fread(&chunkSize, 4, 1, file) != 1) break;

        if (strncmp(chunkId, "fmt ", 4) == 0) {
            long chunkStart = ftell(file);
            if (readFmtChunk(file, result.sampleRate, result.numChannels, result.bitDepth)) {
                foundFmt = true;
            }
            fseek(file, chunkStart + chunkSize, SEEK_SET);
        }
        else if (strncmp(chunkId, "data", 4) == 0) {
            if (foundFmt) {
                if (readDataChunk(file, result.samples, result.numChannels, result.bitDepth)) {
                    foundData = true;
                }
            } else {
                fseek(file, chunkSize, SEEK_CUR);
            }
        }
        else if (strncmp(chunkId, "smpl", 4) == 0) {
            readSmplChunk(file, chunkSize, result.loopStart, result.loopEnd, result.hasLoop);
        }
        else {
            fseek(file, chunkSize, SEEK_CUR);
        }

        if (chunkSize % 2 != 0) {
            fseek(file, 1, SEEK_CUR);
        }
    }

    fclose(file);

    if (foundFmt && foundData) {
        result.success = true;
    } else {
        result.errorMessage = "Missing required chunks";
    }

    return result;
}

bool WavLoader::readRiffHeader(FILE* file, uint32_t& fileSize) {
    char riff[4];
    if (fread(riff, 1, 4, file) != 4) return false;
    if (strncmp(riff, "RIFF", 4) != 0) return false;

    if (fread(&fileSize, 4, 1, file) != 1) return false;

    char wave[4];
    if (fread(wave, 1, 4, file) != 4) return false;
    if (strncmp(wave, "WAVE", 4) != 0) return false;

    return true;
}

bool WavLoader::readFmtChunk(FILE* file, int& sampleRate, int& numChannels, int& bitDepth) {
    uint16_t audioFormat;
    if (fread(&audioFormat, 2, 1, file) != 1) return false;
    if (audioFormat != 1) return false;

    uint16_t channels;
    if (fread(&channels, 2, 1, file) != 1) return false;
    numChannels = channels;

    uint32_t rate;
    if (fread(&rate, 4, 1, file) != 1) return false;
    sampleRate = rate;

    uint32_t byteRate;
    if (fread(&byteRate, 4, 1, file) != 1) return false;

    uint16_t blockAlign;
    if (fread(&blockAlign, 2, 1, file) != 1) return false;

    uint16_t bits;
    if (fread(&bits, 2, 1, file) != 1) return false;
    bitDepth = bits;

    return true;
}

bool WavLoader::readDataChunk(FILE* file, std::vector<float>& samples, int numChannels, int bitDepth) {
    long dataStart = ftell(file);
    fseek(file, 0, SEEK_END);
    long fileEnd = ftell(file);
    fseek(file, dataStart, SEEK_SET);

    long dataSize = fileEnd - dataStart;
    int bytesPerSample = bitDepth / 8;
    int totalSamples = dataSize / bytesPerSample;

    std::vector<uint8_t> buffer(dataSize);
    if (fread(buffer.data(), 1, dataSize, file) != static_cast<size_t>(dataSize)) {
        return false;
    }

    int outputSamples = totalSamples / numChannels;
    samples.resize(outputSamples);

    if (bitDepth == 16) {
        const int16_t* data16 = reinterpret_cast<const int16_t*>(buffer.data());
        if (numChannels == 1) {
            for (int i = 0; i < outputSamples; ++i) {
                samples[i] = data16[i] / 32768.0f;
            }
        } else if (numChannels == 2) {
            for (int i = 0; i < outputSamples; ++i) {
                float left = data16[i * 2] / 32768.0f;
                float right = data16[i * 2 + 1] / 32768.0f;
                samples[i] = (left + right) * 0.5f;
            }
        }
    }
    else if (bitDepth == 24) {
        const uint8_t* data24 = buffer.data();
        if (numChannels == 1) {
            for (int i = 0; i < outputSamples; ++i) {
                int32_t sample = read24BitSample(&data24[i * 3]);
                samples[i] = sample / 8388608.0f;
            }
        } else if (numChannels == 2) {
            for (int i = 0; i < outputSamples; ++i) {
                int32_t left = read24BitSample(&data24[i * 6]);
                int32_t right = read24BitSample(&data24[i * 6 + 3]);
                samples[i] = ((left / 8388608.0f) + (right / 8388608.0f)) * 0.5f;
            }
        }
    }
    else {
        return false;
    }

    return true;
}

bool WavLoader::readSmplChunk(FILE* file, uint32_t chunkSize, uint32_t& loopStart, uint32_t& loopEnd, bool& hasLoop) {
    if (chunkSize < 36) {
        fseek(file, chunkSize, SEEK_CUR);
        return false;
    }

    fseek(file, 28, SEEK_CUR);

    uint32_t numSampleLoops;
    if (fread(&numSampleLoops, 4, 1, file) != 1) return false;

    if (numSampleLoops > 0) {
        fseek(file, 4, SEEK_CUR);

        uint32_t loopType;
        if (fread(&loopType, 4, 1, file) != 1) return false;

        if (fread(&loopStart, 4, 1, file) != 1) return false;
        if (fread(&loopEnd, 4, 1, file) != 1) return false;

        hasLoop = true;
    }

    long remaining = chunkSize - (ftell(file) - (ftell(file) - chunkSize));
    if (remaining > 0) {
        fseek(file, remaining, SEEK_CUR);
    }

    return hasLoop;
}

int32_t WavLoader::read24BitSample(const uint8_t* bytes) {
    int32_t sample = (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
    if (sample & 0x800000) {
        sample |= 0xFF000000;
    }
    return sample;
}

} // namespace synthcore
