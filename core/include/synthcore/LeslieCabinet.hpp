#pragma once

#include "synthcore/Types.hpp"

namespace synthcore {

class LeslieCabinet {
public:
    LeslieCabinet();

    void setRate(float rateHz) noexcept;
    void setDepth(float depth) noexcept;

    float process(float input, SampleRate sampleRate) noexcept;
    void reset() noexcept;

private:
    float rateHz_;
    float depth_;
    float phase_;
};

} // namespace synthcore