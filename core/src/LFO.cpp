#include "synthcore/LFO.hpp"
#include <cmath>

namespace synthcore {

LFO::LFO()
    : waveform_(LFOWaveform::Sine)
    , phase_(0.0)
    , phaseIncrement_(0.0)
    , lastSH_(0.0f)
    , shCounter_(0)
    , rngState_(0x12345678)
{
}

void LFO::setRate(float hz, float sampleRate) noexcept {
    phaseIncrement_ = hz / sampleRate;
}

void LFO::setWaveform(LFOWaveform waveform) noexcept {
    waveform_ = waveform;
}

void LFO::reset() noexcept {
    phase_ = 0.0;
    lastSH_ = 0.0f;
    shCounter_ = 0;
}

float LFO::process() noexcept {
    float output = 0.0f;
    
    switch (waveform_) {
        case LFOWaveform::Sine:
            output = processSine();
            break;
        case LFOWaveform::Triangle:
            output = processTriangle();
            break;
        case LFOWaveform::Sawtooth:
            output = processSawtooth();
            break;
        case LFOWaveform::ReverseSawtooth:
            output = processReverseSawtooth();
            break;
        case LFOWaveform::Square:
            output = processSquare();
            break;
        case LFOWaveform::SampleAndHold:
            output = processSampleAndHold();
            break;
    }
    
    phase_ += phaseIncrement_;
    if (phase_ >= 1.0) {
        phase_ -= 1.0;
    }
    
    return output;
}

float LFO::processSine() noexcept {
    return static_cast<float>(std::sin(phase_ * 2.0 * 3.14159265358979323846));
}

float LFO::processTriangle() noexcept {
    if (phase_ < 0.5) {
        return static_cast<float>(4.0 * phase_ - 1.0);
    } else {
        return static_cast<float>(3.0 - 4.0 * phase_);
    }
}

float LFO::processSawtooth() noexcept {
    return static_cast<float>(2.0 * phase_ - 1.0);
}

float LFO::processReverseSawtooth() noexcept {
    return static_cast<float>(1.0 - 2.0 * phase_);
}

float LFO::processSquare() noexcept {
    return phase_ < 0.5 ? 1.0f : -1.0f;
}

float LFO::processSampleAndHold() noexcept {
    if (shCounter_ == 0) {
        lastSH_ = xorshift32() * 2.0f - 1.0f;
        shCounter_ = 100;
    }
    shCounter_--;
    return lastSH_;
}

float LFO::xorshift32() noexcept {
    rngState_ ^= rngState_ << 13;
    rngState_ ^= rngState_ >> 17;
    rngState_ ^= rngState_ << 5;
    return static_cast<float>(rngState_) / 4294967295.0f;
}

} // namespace synthcore
