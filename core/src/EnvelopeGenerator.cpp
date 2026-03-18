#include "synthcore/EnvelopeGenerator.hpp"
#include <algorithm>
#include <cmath>

namespace synthcore {

EnvelopeGenerator::EnvelopeGenerator()
    : stage_(EnvelopeStage::Idle)
    , level_(0.0f)
    , sampleRate_(44100.0f)
    , delayTime_(0.0f)
    , attackTime_(0.01f)
    , holdTime_(0.0f)
    , decayTime_(0.1f)
    , sustainLevel_(0.7f)
    , releaseTime_(0.3f)
    , delayIncrement_(0.0f)
    , attackIncrement_(0.0f)
    , holdIncrement_(0.0f)
    , decayIncrement_(0.0f)
    , releaseIncrement_(0.0f)
    , stageProgress_(0.0f)
{
}

void EnvelopeGenerator::setParameters(float delayTime, float attackTime, float holdTime,
                                     float decayTime, float sustainLevel, float releaseTime) {
    delayTime_ = std::max(0.0f, delayTime);
    attackTime_ = std::max(0.001f, attackTime);
    holdTime_ = std::max(0.0f, holdTime);
    decayTime_ = std::max(0.001f, decayTime);
    sustainLevel_ = std::clamp(sustainLevel, 0.0f, 1.0f);
    releaseTime_ = std::max(0.001f, releaseTime);
}

void EnvelopeGenerator::noteOn(float sampleRate) noexcept {
    sampleRate_ = sampleRate;
    level_ = 0.0f;
    stageProgress_ = 0.0f;
    
    calculateIncrements();
    
    if (delayTime_ > 0.0f) {
        stage_ = EnvelopeStage::Delay;
    } else {
        stage_ = EnvelopeStage::Attack;
    }
}

void EnvelopeGenerator::noteOff() noexcept {
    stage_ = EnvelopeStage::Release;
    stageProgress_ = 0.0f;
}

float EnvelopeGenerator::process() noexcept {
    switch (stage_) {
        case EnvelopeStage::Idle:
            return 0.0f;
            
        case EnvelopeStage::Delay:
            stageProgress_ += delayIncrement_;
            if (stageProgress_ >= 1.0f) {
                stage_ = EnvelopeStage::Attack;
                stageProgress_ = 0.0f;
            }
            return 0.0f;
            
        case EnvelopeStage::Attack:
            level_ += attackIncrement_;
            if (level_ >= 1.0f) {
                level_ = 1.0f;
                stage_ = holdTime_ > 0.0f ? EnvelopeStage::Hold : EnvelopeStage::Decay;
                stageProgress_ = 0.0f;
            }
            return level_;
            
        case EnvelopeStage::Hold:
            stageProgress_ += holdIncrement_;
            if (stageProgress_ >= 1.0f) {
                stage_ = EnvelopeStage::Decay;
                stageProgress_ = 0.0f;
            }
            return 1.0f;
            
        case EnvelopeStage::Decay:
            level_ -= decayIncrement_;
            if (level_ <= sustainLevel_) {
                level_ = sustainLevel_;
                stage_ = EnvelopeStage::Sustain;
            }
            return level_;
            
        case EnvelopeStage::Sustain:
            return sustainLevel_;
            
        case EnvelopeStage::Release:
            level_ -= releaseIncrement_;
            if (level_ <= 0.0f) {
                level_ = 0.0f;
                stage_ = EnvelopeStage::Idle;
            }
            return level_;
    }
    
    return 0.0f;
}

bool EnvelopeGenerator::isIdle() const noexcept {
    return stage_ == EnvelopeStage::Idle;
}

void EnvelopeGenerator::calculateIncrements() noexcept {
    delayIncrement_ = delayTime_ > 0.0f ? 1.0f / (delayTime_ * sampleRate_) : 0.0f;
    attackIncrement_ = 1.0f / (attackTime_ * sampleRate_);
    holdIncrement_ = holdTime_ > 0.0f ? 1.0f / (holdTime_ * sampleRate_) : 0.0f;
    decayIncrement_ = (1.0f - sustainLevel_) / (decayTime_ * sampleRate_);
    releaseIncrement_ = sustainLevel_ / (releaseTime_ * sampleRate_);
}

} // namespace synthcore
