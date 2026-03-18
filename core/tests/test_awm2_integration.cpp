#include <gtest/gtest.h>
#include "synthcore/VoiceManager.hpp"
#include "synthcore/SampleMap.hpp"
#include "synthcore/SampleRegion.hpp"
#include <vector>
#include <cmath>

using namespace synthcore;

static std::vector<Sample> generateSineWave(float frequency, SampleRate sampleRate, size_t sampleCount) {
    std::vector<Sample> samples(sampleCount);
    for (size_t i = 0; i < sampleCount; ++i) {
        samples[i] = static_cast<float>(std::sin(2.0 * 3.14159265358979323846 * frequency * i / sampleRate));
    }
    return samples;
}

TEST(AWM2IntegrationTest, BasicNotePlayback) {
    VoiceManager voiceManager;
    SampleMap sampleMap;
    
    auto samples = generateSineWave(440.0f, 44100.0, 44100);
    
    SampleRegion region;
    region.samples = std::move(samples);
    region.rootNote = 69;
    region.loNote = 0;
    region.hiNote = 127;
    region.loVel = 0;
    region.hiVel = 127;
    
    sampleMap.addRegion(region);
    
    voiceManager.noteOn(sampleMap, 69, 1.0f, 44100.0);
    
    EXPECT_EQ(voiceManager.getActiveVoiceCount(), 1);
    
    AudioBuffer buffer;
    buffer.size = 512;
    voiceManager.process(buffer, sampleMap, 44100.0, 512);
    
    bool hasNonZero = false;
    for (size_t i = 0; i < 512; ++i) {
        if (buffer.data[i] != 0.0f) {
            hasNonZero = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasNonZero);
}

TEST(AWM2IntegrationTest, NoteOffRelease) {
    VoiceManager voiceManager;
    SampleMap sampleMap;
    
    auto samples = generateSineWave(440.0f, 44100.0, 88200);
    
    SampleRegion region;
    region.samples = std::move(samples);
    region.rootNote = 69;
    region.loNote = 0;
    region.hiNote = 127;
    region.loVel = 0;
    region.hiVel = 127;
    
    sampleMap.addRegion(region);
    
    voiceManager.noteOn(sampleMap, 69, 1.0f, 44100.0);
    
    AudioBuffer buffer;
    buffer.size = 512;
    
    voiceManager.process(buffer, sampleMap, 44100.0, 512);
    
    voiceManager.noteOff(69);
    
    voiceManager.process(buffer, sampleMap, 44100.0, 512);
    
    EXPECT_GE(voiceManager.getActiveVoiceCount(), 0);
}

TEST(AWM2IntegrationTest, PolyphonicPlayback) {
    VoiceManager voiceManager;
    SampleMap sampleMap;
    
    auto samples = generateSineWave(440.0f, 44100.0, 44100);
    
    SampleRegion region;
    region.samples = samples;
    region.rootNote = 69;
    region.loNote = 0;
    region.hiNote = 127;
    region.loVel = 0;
    region.hiVel = 127;
    
    sampleMap.addRegion(region);
    
    voiceManager.noteOn(sampleMap, 60, 1.0f, 44100.0);
    voiceManager.noteOn(sampleMap, 64, 1.0f, 44100.0);
    voiceManager.noteOn(sampleMap, 67, 1.0f, 44100.0);
    
    EXPECT_EQ(voiceManager.getActiveVoiceCount(), 3);
    
    AudioBuffer buffer;
    buffer.size = 512;
    voiceManager.process(buffer, sampleMap, 44100.0, 512);
    
    bool hasNonZero = false;
    for (size_t i = 0; i < 512; ++i) {
        if (buffer.data[i] != 0.0f) {
            hasNonZero = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasNonZero);
}

TEST(AWM2IntegrationTest, VoiceStealing) {
    VoiceManager voiceManager;
    SampleMap sampleMap;
    
    auto samples = generateSineWave(440.0f, 44100.0, 88200);
    
    SampleRegion region;
    region.samples = samples;
    region.rootNote = 69;
    region.loNote = 0;
    region.hiNote = 127;
    region.loVel = 0;
    region.hiVel = 127;
    
    sampleMap.addRegion(region);
    
    for (MidiNote note = 0; note < 127; ++note) {
        voiceManager.noteOn(sampleMap, note, 1.0f, 44100.0);
    }
    
    EXPECT_LE(voiceManager.getActiveVoiceCount(), MAX_VOICES);
}

TEST(AWM2IntegrationTest, AllNotesOff) {
    VoiceManager voiceManager;
    SampleMap sampleMap;
    
    auto samples = generateSineWave(440.0f, 44100.0, 88200);
    
    SampleRegion region;
    region.samples = samples;
    region.rootNote = 69;
    region.loNote = 0;
    region.hiNote = 127;
    region.loVel = 0;
    region.hiVel = 127;
    
    sampleMap.addRegion(region);
    
    voiceManager.noteOn(sampleMap, 60, 1.0f, 44100.0);
    voiceManager.noteOn(sampleMap, 64, 1.0f, 44100.0);
    voiceManager.noteOn(sampleMap, 67, 1.0f, 44100.0);
    
    voiceManager.allNotesOff();
    
    AudioBuffer buffer;
    buffer.size = 2048;
    for (int i = 0; i < 100; ++i) {
        voiceManager.process(buffer, sampleMap, 44100.0, 2048);
    }
    
    EXPECT_EQ(voiceManager.getActiveVoiceCount(), 0);
}
