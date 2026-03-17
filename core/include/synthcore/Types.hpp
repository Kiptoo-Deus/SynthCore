#pragma once

#include <cstdint>

namespace synthcore {

using Sample = float;
using SampleRate = double;

constexpr int MAX_VOICES = 64;
constexpr int MAX_ELEMENTS_PER_VOICE = 8;
constexpr int MAX_POLYPHONY = 64;

constexpr double A4_FREQUENCY = 440.0;

struct MidiNote {
    uint8_t note;
    uint8_t velocity;
    uint8_t channel;
};

struct AudioBuffer {
    Sample* left;
    Sample* right;
    int numFrames;
};

} // namespace synthcore
