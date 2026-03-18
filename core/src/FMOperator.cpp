#include "synthcore/FMOperator.hpp"
#include <cmath>
#include <algorithm>

namespace synthcore {

FMOperator::FMOperator()
    : phase_(0.0f)
    , frequencyRatio_(1.0f)
    , detune_(0.0f)
    , baseFrequency_(0.0f)
    , outputLevel_(1.0f)
    , feedback_(0.0f)
    , feedbackSample_(0.0f)
    , envelopeLevel_(0.0f)
    , velocity_(0.0f)
    , envStage_(EnvStage::Idle)
    , attackRate_(0.01f)
    , decayRate_(0.01f)
    , sustainLevel_(0.7f)
    , releaseRate_(0.01f)
    , stageProgress_(0.0f)
    , sampleRate_(44100.0)
{
}

void FMOperator::setEnvelope(float attack, float decay, float sustain, float release) noexcept {
    attackRate_ = attack > 0.0f ? 1.0f / (attack * static_cast<float>(sampleRate_)) : 1.0f;
    decayRate_ = decay > 0.0f ? 1.0f / (decay * static_cast<float>(sampleRate_)) : 1.0f;
    sustainLevel_ = std::clamp(sustain, 0.0f, 1.0f);
    releaseRate_ = release > 0.0f ? 1.0f / (release * static_cast<float>(sampleRate_)) : 1.0f;
}

void FMOperator::noteOn(float baseFrequency, float velocity, SampleRate sampleRate) noexcept {
    baseFrequency_ = baseFrequency;
    velocity_ = velocity;
    sampleRate_ = sampleRate;
    envStage_ = EnvStage::Attack;
    stageProgress_ = 0.0f;
    envelopeLevel_ = 0.0f;
    phase_ = 0.0f;
    feedbackSample_ = 0.0f;
}

void FMOperator::noteOff() noexcept {
    if (envStage_ != EnvStage::Idle) {
        envStage_ = EnvStage::Release;
        stageProgress_ = 0.0f;
    }
}

void FMOperator::reset() noexcept {
    phase_ = 0.0f;
    baseFrequency_ = 0.0f;
    envelopeLevel_ = 0.0f;
    velocity_ = 0.0f;
    envStage_ = EnvStage::Idle;
    stageProgress_ = 0.0f;
    feedbackSample_ = 0.0f;
}

Sample FMOperator::process(Sample modulation, SampleRate sampleRate) noexcept {
    (void)sampleRate;
    
    if (envStage_ == EnvStage::Idle) {
        return 0.0f;
    }
    
    processEnvelope();
    
    float phaseModulation = modulation + (feedbackSample_ * feedback_);
    float currentPhase = phase_ + phaseModulation;
    
    Sample output = std::sin(currentPhase * 2.0f * 3.14159265358979323846f);
    output *= envelopeLevel_ * outputLevel_ * velocity_;
    
    feedbackSample_ = output;
    
    float increment = calculateIncrement();
    phase_ += increment;
    
    if (phase_ >= 1.0f) {
        phase_ -= 1.0f;
    }
    
    return output;
}

bool FMOperator::isActive() const noexcept {
    return envStage_ != EnvStage::Idle;
}

void FMOperator::processEnvelope() noexcept {
    switch (envStage_) {
        case EnvStage::Attack:
            envelopeLevel_ += attackRate_;
            if (envelopeLevel_ >= 1.0f) {
                envelopeLevel_ = 1.0f;
                envStage_ = EnvStage::Decay;
                stageProgress_ = 0.0f;
            }
            break;
            
        case EnvStage::Decay:
            envelopeLevel_ -= (1.0f - sustainLevel_) * decayRate_;
            if (envelopeLevel_ <= sustainLevel_) {
                envelopeLevel_ = sustainLevel_;
                envStage_ = EnvStage::Sustain;
            }
            break;
            
        case EnvStage::Sustain:
            envelopeLevel_ = sustainLevel_;
            break;
            
        case EnvStage::Release:
            envelopeLevel_ -= envelopeLevel_ * releaseRate_;
            if (envelopeLevel_ <= 0.001f) {
                envelopeLevel_ = 0.0f;
                envStage_ = EnvStage::Idle;
            }
            break;
            
        case EnvStage::Idle:
            envelopeLevel_ = 0.0f;
            break;
    }
}

float FMOperator::calculateIncrement() const noexcept {
    float frequency = baseFrequency_ * frequencyRatio_ * (1.0f + detune_);
    return static_cast<float>(frequency / sampleRate_);
}

} // namespace synthcore
