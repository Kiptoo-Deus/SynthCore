#include <gtest/gtest.h>
#include "synthcore/WavLoader.hpp"
#include <cmath>
#include <cstdio>
#include <cstring>

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#pragma warning(disable: 4244)
#endif

namespace {

void writeWavFile(const std::string& filename, int sampleRate, int numChannels, 
                  int bitDepth, const std::vector<float>& samples) {
    FILE* file = fopen(filename.c_str(), "wb");
    if (!file) return;

    int bytesPerSample = bitDepth / 8;
    int dataSize = static_cast<int>(samples.size()) * numChannels * bytesPerSample;
    int fileSize = 36 + dataSize;

    fwrite("RIFF", 1, 4, file);
    fwrite(&fileSize, 4, 1, file);
    fwrite("WAVE", 1, 4, file);

    fwrite("fmt ", 1, 4, file);
    uint32_t fmtSize = 16;
    fwrite(&fmtSize, 4, 1, file);
    uint16_t audioFormat = 1;
    fwrite(&audioFormat, 2, 1, file);
    uint16_t channels = numChannels;
    fwrite(&channels, 2, 1, file);
    uint32_t rate = sampleRate;
    fwrite(&rate, 4, 1, file);
    uint32_t byteRate = sampleRate * numChannels * bytesPerSample;
    fwrite(&byteRate, 4, 1, file);
    uint16_t blockAlign = numChannels * bytesPerSample;
    fwrite(&blockAlign, 2, 1, file);
    uint16_t bits = bitDepth;
    fwrite(&bits, 2, 1, file);

    fwrite("data", 1, 4, file);
    fwrite(&dataSize, 4, 1, file);

    if (bitDepth == 16) {
        for (size_t i = 0; i < samples.size(); ++i) {
            int16_t sample = static_cast<int16_t>(samples[i] * 32767.0f);
            for (int ch = 0; ch < numChannels; ++ch) {
                fwrite(&sample, 2, 1, file);
            }
        }
    } else if (bitDepth == 24) {
        for (size_t i = 0; i < samples.size(); ++i) {
            int32_t sample32 = static_cast<int32_t>(samples[i] * 8388607.0f);
            uint8_t bytes[3];
            bytes[0] = sample32 & 0xFF;
            bytes[1] = (sample32 >> 8) & 0xFF;
            bytes[2] = (sample32 >> 16) & 0xFF;
            for (int ch = 0; ch < numChannels; ++ch) {
                fwrite(bytes, 1, 3, file);
            }
        }
    }

    fclose(file);
}

std::vector<float> generateSineWave(int sampleRate, float frequency, float duration) {
    int numSamples = static_cast<int>(sampleRate * duration);
    std::vector<float> samples(numSamples);
    
    for (int i = 0; i < numSamples; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        samples[i] = std::sin(2.0f * 3.14159265359f * frequency * t);
    }
    
    return samples;
}

}

TEST(WavLoaderTest, Load16BitMonoWav) {
    auto samples = generateSineWave(44100, 440.0f, 2.0f);
    writeWavFile("test_16bit_mono.wav", 44100, 1, 16, samples);

    auto result = synthcore::WavLoader::load("test_16bit_mono.wav");

    ASSERT_TRUE(result.success);
    EXPECT_EQ(result.sampleRate, 44100);
    EXPECT_EQ(result.numChannels, 1);
    EXPECT_EQ(result.bitDepth, 16);
    EXPECT_EQ(result.samples.size(), samples.size());
    EXPECT_FALSE(result.hasLoop);

    for (size_t i = 0; i < std::min(size_t(100), result.samples.size()); ++i) {
        EXPECT_GE(result.samples[i], -1.0f);
        EXPECT_LE(result.samples[i], 1.0f);
    }

    std::remove("test_16bit_mono.wav");
}

TEST(WavLoaderTest, Load24BitMonoWav) {
    auto samples = generateSineWave(48000, 1000.0f, 1.0f);
    writeWavFile("test_24bit_mono.wav", 48000, 1, 24, samples);

    auto result = synthcore::WavLoader::load("test_24bit_mono.wav");

    ASSERT_TRUE(result.success);
    EXPECT_EQ(result.sampleRate, 48000);
    EXPECT_EQ(result.numChannels, 1);
    EXPECT_EQ(result.bitDepth, 24);
    EXPECT_EQ(result.samples.size(), samples.size());

    std::remove("test_24bit_mono.wav");
}

TEST(WavLoaderTest, Load16BitStereoSummedToMono) {
    auto samples = generateSineWave(44100, 440.0f, 1.0f);
    writeWavFile("test_16bit_stereo.wav", 44100, 2, 16, samples);

    auto result = synthcore::WavLoader::load("test_16bit_stereo.wav");

    ASSERT_TRUE(result.success);
    EXPECT_EQ(result.numChannels, 2);
    EXPECT_EQ(result.samples.size(), samples.size());

    for (float sample : result.samples) {
        EXPECT_GE(sample, -1.0f);
        EXPECT_LE(sample, 1.0f);
    }

    std::remove("test_16bit_stereo.wav");
}

TEST(WavLoaderTest, NonExistentFileReturnsError) {
    auto result = synthcore::WavLoader::load("nonexistent_file.wav");

    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.errorMessage.empty());
    EXPECT_EQ(result.samples.size(), 0);
}

TEST(WavLoaderTest, AmplitudeRange) {
    std::vector<float> testSamples = {-1.0f, -0.5f, 0.0f, 0.5f, 1.0f};
    writeWavFile("test_amplitude.wav", 44100, 1, 16, testSamples);

    auto result = synthcore::WavLoader::load("test_amplitude.wav");

    ASSERT_TRUE(result.success);
    
    float minSample = *std::min_element(result.samples.begin(), result.samples.end());
    float maxSample = *std::max_element(result.samples.begin(), result.samples.end());
    
    EXPECT_GE(minSample, -1.0f);
    EXPECT_LE(maxSample, 1.0f);

    std::remove("test_amplitude.wav");
}
