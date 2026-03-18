#pragma once

namespace synthcore {

enum class FilterType {
    LowPass12,
    HighPass12,
    BandPass,
    Notch,
    LowShelf,
    HighShelf,
    LowPass24
};

class BiquadFilter {
public:
    BiquadFilter();
    
    void setParameters(FilterType type, float cutoffHz, float Q, float sampleRate) noexcept;
    float process(float input) noexcept;
    void reset() noexcept;
    
private:
    float b0_, b1_, b2_;
    float a1_, a2_;
    float w1_, w2_;
    
    FilterType type_;
    BiquadFilter* secondStage_;
    
    void calculateCoefficients(FilterType type, float cutoffHz, float Q, float sampleRate) noexcept;
};

} // namespace synthcore
