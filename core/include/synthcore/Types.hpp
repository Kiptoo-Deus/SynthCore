#pragma once

#include <cstdint>

namespace synthcore {

using Sample = float;
using SampleRate = double;
using MidiNote = uint8_t;

constexpr int MAX_VOICES = 64;
constexpr int MAX_ELEMENTS_PER_VOICE = 8;
constexpr int MAX_POLYPHONY = 64;

constexpr double A4_FREQUENCY = 440.0;

struct AudioBuffer {
    Sample data[8192];
    size_t size;
};

} // namespace synthcore
