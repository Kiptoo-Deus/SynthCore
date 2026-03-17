#pragma once

#include <vector>
#include <cstdint>

namespace synthcore {

enum class LoopType {
    None,
    Sustain,
    Release
};

struct SampleRegion {
    std::vector<float> samples;
    int sampleRate;
    uint8_t rootNote;
    uint8_t loNote;
    uint8_t hiNote;
    uint8_t loVel;
    uint8_t hiVel;
    uint32_t loopStart;
    uint32_t loopEnd;
    bool hasLoop;
    LoopType loopType;
    int rrGroup;

    SampleRegion()
        : sampleRate(44100)
        , rootNote(60)
        , loNote(0)
        , hiNote(127)
        , loVel(0)
        , hiVel(127)
        , loopStart(0)
        , loopEnd(0)
        , hasLoop(false)
        , loopType(LoopType::None)
        , rrGroup(0)
    {}
};

} // namespace synthcore
