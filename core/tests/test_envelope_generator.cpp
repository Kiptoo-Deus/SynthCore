#include <gtest/gtest.h>
#include "synthcore/EnvelopeGenerator.hpp"

TEST(EnvelopeGeneratorTest, PianoEnvelope) {
    synthcore::EnvelopeGenerator env;
    env.setParameters(0.0f, 0.002f, 0.0f, 0.2f, 0.7f, 0.3f);
    
    env.noteOn(44100.0f);
    
    EXPECT_FALSE(env.isIdle());
    
    int attackSamples = static_cast<int>(0.002f * 44100.0f);
    for (int i = 0; i < attackSamples + 100; ++i) {
        env.process();
    }
    
    float level = env.process();
    EXPECT_GE(level, 0.99f);
    EXPECT_LE(level, 1.0f);
    
    int decaySamples = static_cast<int>(0.2f * 44100.0f);
    for (int i = 0; i < decaySamples + 1000; ++i) {
        env.process();
    }
    
    level = env.process();
    EXPECT_GE(level, 0.69f);
    EXPECT_LE(level, 0.71f);
    
    env.noteOff();
    
    int releaseSamples = static_cast<int>(0.3f * 44100.0f);
    for (int i = 0; i < releaseSamples + 1000; ++i) {
        env.process();
    }
    
    EXPECT_TRUE(env.isIdle());
    EXPECT_FLOAT_EQ(env.process(), 0.0f);
}

TEST(EnvelopeGeneratorTest, AttackReachesOne) {
    synthcore::EnvelopeGenerator env;
    env.setParameters(0.0f, 0.01f, 0.0f, 0.1f, 0.5f, 0.1f);
    
    env.noteOn(44100.0f);
    
    int attackSamples = static_cast<int>(0.01f * 44100.0f);
    float maxLevel = 0.0f;
    
    for (int i = 0; i < attackSamples + 100; ++i) {
        float level = env.process();
        maxLevel = std::max(maxLevel, level);
    }
    
    EXPECT_GE(maxLevel, 0.99f);
}

TEST(EnvelopeGeneratorTest, SustainLevel) {
    synthcore::EnvelopeGenerator env;
    env.setParameters(0.0f, 0.01f, 0.0f, 0.1f, 0.6f, 0.1f);
    
    env.noteOn(44100.0f);
    
    int totalSamples = static_cast<int>((0.01f + 0.1f + 0.5f) * 44100.0f);
    for (int i = 0; i < totalSamples; ++i) {
        env.process();
    }
    
    float level = env.process();
    EXPECT_GE(level, 0.59f);
    EXPECT_LE(level, 0.61f);
}

TEST(EnvelopeGeneratorTest, ReleaseCompletes) {
    synthcore::EnvelopeGenerator env;
    env.setParameters(0.0f, 0.001f, 0.0f, 0.001f, 0.5f, 0.05f);
    
    env.noteOn(44100.0f);
    
    for (int i = 0; i < 1000; ++i) {
        env.process();
    }
    
    env.noteOff();
    
    int releaseSamples = static_cast<int>(0.05f * 44100.0f);
    for (int i = 0; i < releaseSamples + 1000; ++i) {
        env.process();
    }
    
    EXPECT_TRUE(env.isIdle());
}

TEST(EnvelopeGeneratorTest, IdleReturnsZero) {
    synthcore::EnvelopeGenerator env;
    
    EXPECT_TRUE(env.isIdle());
    EXPECT_FLOAT_EQ(env.process(), 0.0f);
}
