#include "synthcore/BiquadFilter.hpp"
#include <cmath>
#include <algorithm>

namespace synthcore {

BiquadFilter::BiquadFilter()
    : b0_(1.0f)
    , b1_(0.0f)
    , b2_(0.0f)
    , a1_(0.0f)
    , a2_(0.0f)
    , w1_(0.0f)
    , w2_(0.0f)
    , type_(FilterType::LowPass12)
    , secondStage_(nullptr)
{
}

void BiquadFilter::setParameters(FilterType type, float cutoffHz, float Q, float sampleRate) noexcept {
    type_ = type;
    
    if (type == FilterType::LowPass24) {
        calculateCoefficients(FilterType::LowPass12, cutoffHz, Q, sampleRate);
        
        if (!secondStage_) {
            secondStage_ = new BiquadFilter();
        }
        secondStage_->setParameters(FilterType::LowPass12, cutoffHz, Q, sampleRate);
    } else {
        calculateCoefficients(type, cutoffHz, Q, sampleRate);
    }
}

float BiquadFilter::process(float input) noexcept {
    float w0 = input - a1_ * w1_ - a2_ * w2_;
    float output = b0_ * w0 + b1_ * w1_ + b2_ * w2_;
    
    w2_ = w1_;
    w1_ = w0;
    
    if (type_ == FilterType::LowPass24 && secondStage_) {
        output = secondStage_->process(output);
    }
    
    return output;
}

void BiquadFilter::reset() noexcept {
    w1_ = 0.0f;
    w2_ = 0.0f;
    
    if (secondStage_) {
        secondStage_->reset();
    }
}

void BiquadFilter::calculateCoefficients(FilterType type, float cutoffHz, float Q, float sampleRate) noexcept {
    cutoffHz = std::clamp(cutoffHz, 20.0f, sampleRate * 0.45f);
    Q = std::max(0.1f, Q);
    
    float omega = 2.0f * 3.14159265358979323846f * cutoffHz / sampleRate;
    float sinOmega = std::sin(omega);
    float cosOmega = std::cos(omega);
    float alpha = sinOmega / (2.0f * Q);
    
    float a0 = 1.0f + alpha;
    
    switch (type) {
        case FilterType::LowPass12:
            b0_ = ((1.0f - cosOmega) / 2.0f) / a0;
            b1_ = (1.0f - cosOmega) / a0;
            b2_ = ((1.0f - cosOmega) / 2.0f) / a0;
            a1_ = (-2.0f * cosOmega) / a0;
            a2_ = (1.0f - alpha) / a0;
            break;
            
        case FilterType::HighPass12:
            b0_ = ((1.0f + cosOmega) / 2.0f) / a0;
            b1_ = -(1.0f + cosOmega) / a0;
            b2_ = ((1.0f + cosOmega) / 2.0f) / a0;
            a1_ = (-2.0f * cosOmega) / a0;
            a2_ = (1.0f - alpha) / a0;
            break;
            
        case FilterType::BandPass:
            b0_ = alpha / a0;
            b1_ = 0.0f;
            b2_ = -alpha / a0;
            a1_ = (-2.0f * cosOmega) / a0;
            a2_ = (1.0f - alpha) / a0;
            break;
            
        case FilterType::Notch:
            b0_ = 1.0f / a0;
            b1_ = (-2.0f * cosOmega) / a0;
            b2_ = 1.0f / a0;
            a1_ = (-2.0f * cosOmega) / a0;
            a2_ = (1.0f - alpha) / a0;
            break;
            
        case FilterType::LowShelf:
        case FilterType::HighShelf:
            b0_ = 1.0f / a0;
            b1_ = 0.0f;
            b2_ = 0.0f;
            a1_ = 0.0f;
            a2_ = 0.0f;
            break;
            
        case FilterType::LowPass24:
            break;
    }
}

} // namespace synthcore
