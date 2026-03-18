#pragma once

#include <cstdint>

namespace synthcore {

enum class LFOWaveform {
    Sine,
    Triangle,
    Sawtooth,
    ReverseSawtooth,
    Square,
    SampleAndHold
};

class LFO {
public:
    LFO();
    
    void setRate(float hz, float sampleRate) noexcept;
    void setWaveform(LFOWaveform waveform) noexcept;
    void reset() noexcept;
    float process() noexcept;
    
private:
    LFOWaveform waveform_;
    double phase_;
    double phaseIncrement_;
    float lastSH_;
    uint32_t shCounter_;
    uint32_t rngState_;
    
    float processSine() noexcept;
    float processTriangle() noexcept;
    float processSawtooth() noexcept;
    float processReverseSawtooth() noexcept;
    float processSquare() noexcept;
    float processSampleAndHold() noexcept;
    float xorshift32() noexcept;
};

} // namespace synthcore
