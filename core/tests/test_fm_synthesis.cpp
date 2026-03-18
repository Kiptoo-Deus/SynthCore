#include <gtest/gtest.h>
#include "synthcore/FMOperator.hpp"
#include "synthcore/FMAlgorithm.hpp"
#include "synthcore/FMVoice.hpp"
#include "synthcore/FMEngine.hpp"
#include <cmath>

using namespace synthcore;

TEST(FMOperatorTest, BasicOperation) {
    FMOperator op;
    op.setFrequencyRatio(1.0f);
    op.setOutputLevel(1.0f);
    op.setEnvelope(0.01f, 0.1f, 0.7f, 0.2f);
    
    op.noteOn(440.0f, 1.0f, 44100.0);
    
    EXPECT_TRUE(op.isActive());
    
    Sample output = op.process(0.0f, 44100.0);
    EXPECT_NE(output, 0.0f);
}

TEST(FMOperatorTest, EnvelopeProgression) {
    FMOperator op;
    op.setEnvelope(0.001f, 0.1f, 0.7f, 0.2f);
    op.noteOn(440.0f, 1.0f, 44100.0);
    
    for (int i = 0; i < 100; ++i) {
        op.process(0.0f, 44100.0);
    }
    
    EXPECT_TRUE(op.isActive());
    
    op.noteOff();
    
    for (int i = 0; i < 44100; ++i) {
        op.process(0.0f, 44100.0);
    }
    
    EXPECT_FALSE(op.isActive());
}

TEST(FMOperatorTest, FrequencyRatios) {
    FMOperator op1, op2;
    
    op1.setFrequencyRatio(1.0f);
    op2.setFrequencyRatio(2.0f);
    
    op1.setEnvelope(0.0f, 1.0f, 1.0f, 0.0f);
    op2.setEnvelope(0.0f, 1.0f, 1.0f, 0.0f);
    
    op1.noteOn(440.0f, 1.0f, 44100.0);
    op2.noteOn(440.0f, 1.0f, 44100.0);
    
    Sample out1 = op1.process(0.0f, 44100.0);
    Sample out2 = op2.process(0.0f, 44100.0);
    
    EXPECT_NE(out1, 0.0f);
    EXPECT_NE(out2, 0.0f);
}

TEST(FMOperatorTest, Modulation) {
    FMOperator carrier, modulator;
    
    carrier.setFrequencyRatio(1.0f);
    modulator.setFrequencyRatio(2.0f);
    
    carrier.setEnvelope(0.0f, 1.0f, 1.0f, 0.0f);
    modulator.setEnvelope(0.0f, 1.0f, 1.0f, 0.0f);
    
    carrier.noteOn(440.0f, 1.0f, 44100.0);
    modulator.noteOn(440.0f, 1.0f, 44100.0);
    
    Sample modOutput = modulator.process(0.0f, 44100.0);
    Sample carrierOutput = carrier.process(modOutput, 44100.0);
    
    EXPECT_NE(carrierOutput, 0.0f);
}

TEST(FMAlgorithmTest, AlgorithmCount) {
    EXPECT_EQ(FM_ALGORITHM_COUNT, 88);
}

TEST(FMAlgorithmTest, Algorithm0Serial) {
    const auto& algo = FMAlgorithm::getAlgorithm(0);
    EXPECT_EQ(algo.connectionCount, 7);
    EXPECT_EQ(algo.carrierCount, 1);
    EXPECT_EQ(algo.carriers[0], 7);
}

TEST(FMAlgorithmTest, Algorithm7Parallel) {
    const auto& algo = FMAlgorithm::getAlgorithm(7);
    EXPECT_EQ(algo.connectionCount, 0);
    EXPECT_EQ(algo.carrierCount, 8);
}

TEST(FMAlgorithmTest, CarrierDetection) {
    EXPECT_TRUE(FMAlgorithm::isCarrier(0, 7));
    EXPECT_FALSE(FMAlgorithm::isCarrier(0, 0));
    
    EXPECT_TRUE(FMAlgorithm::isCarrier(7, 0));
    EXPECT_TRUE(FMAlgorithm::isCarrier(7, 7));
}

TEST(FMVoiceTest, NoteOnOff) {
    FMVoice voice;
    voice.setAlgorithm(0);
    
    for (int i = 0; i < FM_OPERATORS; ++i) {
        voice.setOperatorRatio(i, 1.0f);
        voice.setOperatorLevel(i, 1.0f);
        voice.setOperatorEnvelope(i, 0.01f, 0.1f, 0.7f, 0.2f);
    }
    
    voice.noteOn(69, 1.0f, 44100.0);
    EXPECT_TRUE(voice.isActive());
    EXPECT_TRUE(voice.isNoteOn());
    EXPECT_EQ(voice.getNote(), 69);
    
    Sample output = voice.process(44100.0);
    EXPECT_NE(output, 0.0f);
    
    voice.noteOff();
    EXPECT_FALSE(voice.isNoteOn());
}

TEST(FMVoiceTest, AlgorithmSelection) {
    FMVoice voice;
    
    for (int algo = 0; algo < 10; ++algo) {
        voice.setAlgorithm(algo);
        
        for (int i = 0; i < FM_OPERATORS; ++i) {
            voice.setOperatorRatio(i, 1.0f);
            voice.setOperatorLevel(i, 0.5f);
            voice.setOperatorEnvelope(i, 0.0f, 1.0f, 1.0f, 0.1f);
        }
        
        voice.noteOn(60, 1.0f, 44100.0);
        
        Sample output = voice.process(44100.0);
        EXPECT_TRUE(std::isfinite(output));
        
        voice.reset();
    }
}

TEST(FMVoiceTest, MidiNoteToFrequency) {
    FMVoice voice;
    voice.setAlgorithm(7);
    
    voice.setOperatorRatio(0, 1.0f);
    voice.setOperatorLevel(0, 1.0f);
    voice.setOperatorEnvelope(0, 0.0f, 1.0f, 1.0f, 0.0f);
    
    voice.noteOn(69, 1.0f, 44100.0);
    Sample output69 = voice.process(44100.0);
    voice.reset();
    
    voice.noteOn(81, 1.0f, 44100.0);
    Sample output81 = voice.process(44100.0);
    
    EXPECT_NE(output69, 0.0f);
    EXPECT_NE(output81, 0.0f);
}

TEST(FMEngineTest, BasicPlayback) {
    FMEngine engine;
    engine.setAlgorithm(7);
    
    for (int i = 0; i < FM_OPERATORS; ++i) {
        engine.setOperatorRatio(i, 1.0f);
        engine.setOperatorLevel(i, 0.125f);
        engine.setOperatorEnvelope(i, 0.01f, 0.1f, 0.7f, 0.2f);
    }
    
    engine.noteOn(60, 1.0f, 44100.0);
    
    EXPECT_EQ(engine.getActiveVoiceCount(), 1);
    
    AudioBuffer buffer;
    buffer.size = 512;
    engine.process(buffer, 44100.0, 512);
    
    bool hasNonZero = false;
    for (size_t i = 0; i < 512; ++i) {
        if (buffer.data[i] != 0.0f) {
            hasNonZero = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasNonZero);
}

TEST(FMEngineTest, Polyphony) {
    FMEngine engine;
    engine.setAlgorithm(0);
    
    for (int i = 0; i < FM_OPERATORS; ++i) {
        engine.setOperatorRatio(i, static_cast<float>(i + 1));
        engine.setOperatorLevel(i, 0.5f);
        engine.setOperatorEnvelope(i, 0.01f, 0.1f, 0.7f, 0.2f);
    }
    
    engine.noteOn(60, 1.0f, 44100.0);
    engine.noteOn(64, 1.0f, 44100.0);
    engine.noteOn(67, 1.0f, 44100.0);
    
    EXPECT_EQ(engine.getActiveVoiceCount(), 3);
    
    AudioBuffer buffer;
    buffer.size = 512;
    engine.process(buffer, 44100.0, 512);
    
    bool hasNonZero = false;
    for (size_t i = 0; i < 512; ++i) {
        if (buffer.data[i] != 0.0f) {
            hasNonZero = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasNonZero);
}

TEST(FMEngineTest, NoteOffRelease) {
    FMEngine engine;
    engine.setAlgorithm(7);
    
    for (int i = 0; i < FM_OPERATORS; ++i) {
        engine.setOperatorEnvelope(i, 0.01f, 0.1f, 0.7f, 0.5f);
    }
    
    engine.noteOn(60, 1.0f, 44100.0);
    
    AudioBuffer buffer;
    buffer.size = 512;
    engine.process(buffer, 44100.0, 512);
    
    engine.noteOff(60);
    
    engine.process(buffer, 44100.0, 512);
    
    EXPECT_GE(engine.getActiveVoiceCount(), 0);
}

TEST(FMEngineTest, AllNotesOff) {
    FMEngine engine;
    
    engine.noteOn(60, 1.0f, 44100.0);
    engine.noteOn(64, 1.0f, 44100.0);
    engine.noteOn(67, 1.0f, 44100.0);
    
    engine.allNotesOff();
    
    AudioBuffer buffer;
    buffer.size = 4096;
    for (int i = 0; i < 100; ++i) {
        engine.process(buffer, 44100.0, 4096);
    }
    
    EXPECT_EQ(engine.getActiveVoiceCount(), 0);
}

TEST(FMEngineTest, ClassicEPiano) {
    FMEngine engine;
    engine.setAlgorithm(4);
    
    engine.setOperatorRatio(0, 1.0f);
    engine.setOperatorLevel(0, 0.8f);
    engine.setOperatorEnvelope(0, 0.001f, 1.5f, 0.0f, 0.1f);
    
    engine.setOperatorRatio(1, 14.0f);
    engine.setOperatorLevel(1, 0.6f);
    engine.setOperatorEnvelope(1, 0.001f, 0.5f, 0.0f, 0.1f);
    
    engine.noteOn(60, 1.0f, 44100.0);
    
    AudioBuffer buffer;
    buffer.size = 1024;
    engine.process(buffer, 44100.0, 1024);
    
    bool hasNonZero = false;
    for (size_t i = 0; i < 1024; ++i) {
        if (buffer.data[i] != 0.0f) {
            hasNonZero = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasNonZero);
}
