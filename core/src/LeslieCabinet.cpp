#include "synthcore/LeslieCabinet.hpp"
#include <cmath>

namespace synthcore {

LeslieCabinet::LeslieCabinet()
    : rateHz_(5.0f)
    , depth_(0.5f)
    , phase_(0.0f)
{
}

void LeslieCabinet::setRate(float rateHz) noexcept {
    rateHz_ = rateHz > 0.0f ? rateHz : 0.1f;
}

void LeslieCabinet::setDepth(float depth) noexcept {
    depth_ = depth < 0.0f ? 0.0f : (depth > 1.0f ? 1.0f : depth);
}

float LeslieCabinet::process(float input, SampleRate sampleRate) noexcept {
    float lfo = 0.5f + 0.5f * std::sin(phase_ * 2.0f * 3.14159265358979323846f);
    phase_ += static_cast<float>(rateHz_ / sampleRate);
    if (phase_ >= 1.0f) {
        phase_ -= 1.0f;
    }

    float modulation = 1.0f - depth_ + depth_ * lfo;
    return input * modulation;
}

void LeslieCabinet::reset() noexcept {
    phase_ = 0.0f;
}

} // namespace synthcore