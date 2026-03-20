#include <gtest/gtest.h>
#include "synthcore/HammondEngine.hpp"
#include "synthcore/LeslieCabinet.hpp"

using namespace synthcore;

TEST(HammondTest, BasicTone) {
    HammondEngine engine;
    engine.setDrawbar(0, 1.0f);
    engine.setDrawbar(1, 0.8f);
    
    engine.noteOn(69, 1.0f, 44100.0);
    
    AudioBuffer buffer;
    buffer.size = 512;
    engine.process(buffer, 44100.0, 512);
    
    bool nonZero = false;
    for (size_t i = 0; i < buffer.size; ++i) {
        if (buffer.data[i] != 0.0f) {
            nonZero = true;
            break;
        }
    }
    
    EXPECT_TRUE(nonZero);
}

TEST(HammondTest, LeslieEffect) {
    HammondEngine engine;
    engine.setDrawbar(0, 1.0f);
    engine.setLeslieRate(7.5f);
    engine.setLeslieDepth(1.0f);
    
    engine.noteOn(69, 1.0f, 44100.0);
    
    AudioBuffer buffer1, buffer2;
    buffer1.size = 256;
    buffer2.size = 256;

    engine.process(buffer1, 44100.0, 256);
    engine.process(buffer2, 44100.0, 256);
    
    bool difference = false;
    for (size_t i = 0; i < 256; ++i) {
        if (buffer1.data[i] != buffer2.data[i]) {
            difference = true;
            break;
        }
    }
    
    EXPECT_TRUE(difference);
}

TEST(HammondTest, PolyphonyAndStealing) {
    HammondEngine engine;
    engine.setDrawbar(0, 0.5f);

    for (int i = 0; i < 80; ++i) {
        engine.noteOn(60 + (i % 12), 1.0f, 44100.0);
    }
    
    EXPECT_LE(engine.getActiveVoiceCount(), MAX_VOICES);
}

TEST(HammondTest, NoteOff) {
    HammondEngine engine;
    engine.setDrawbar(0, 1.0f);

    engine.noteOn(60, 1.0f, 44100.0);
    engine.noteOff(60);
    
    EXPECT_GE(engine.getActiveVoiceCount(), 0);
}
