#include <gtest/gtest.h>
#include "synthcore/SynthCore.hpp"
#include "synthcore/Types.hpp"
#include "synthcore/Voice.hpp"

TEST(SynthCoreSmokeTest, BasicInstantiation) {
    synthcore::SynthCore engine;
    SUCCEED();
}

TEST(TypesTest, SampleTypeSize) {
    EXPECT_EQ(sizeof(synthcore::Sample), 4);
    EXPECT_EQ(sizeof(synthcore::SampleRate), 8);
}

TEST(TypesTest, Constants) {
    EXPECT_EQ(synthcore::MAX_VOICES, 64);
    EXPECT_EQ(synthcore::MAX_ELEMENTS_PER_VOICE, 8);
    EXPECT_EQ(synthcore::MAX_POLYPHONY, 64);
    EXPECT_DOUBLE_EQ(synthcore::A4_FREQUENCY, 440.0);
}

TEST(TypesTest, MidiNoteType) {
    synthcore::MidiNote note = 69;
    EXPECT_EQ(note, 69);
    EXPECT_EQ(sizeof(synthcore::MidiNote), 1);
}

TEST(VoiceTest, BasicInstantiation) {
    synthcore::Voice voice;
    (void)voice;
    SUCCEED();
}
