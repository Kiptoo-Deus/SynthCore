#pragma once

#include <cstdint>

namespace synthcore {

enum class EnvelopeStage {
    Idle,
    Delay,
    Attack,
    Hold,
    Decay,
    Sustain,
    Release
};

class EnvelopeGenerator {
public:
    EnvelopeGenerator();
    
    void setParameters(float delayTime, float attackTime, float holdTime,
                      float decayTime, float sustainLevel, float releaseTime);
    
    void noteOn(float sampleRate) noexcept;
    void noteOff() noexcept;
    float process() noexcept;
    bool isIdle() const noexcept;
    
private:
    EnvelopeStage stage_;
    float level_;
    float sampleRate_;
    
    float delayTime_;
    float attackTime_;
    float holdTime_;
    float decayTime_;
    float sustainLevel_;
    float releaseTime_;
    
    float delayIncrement_;
    float attackIncrement_;
    float holdIncrement_;
    float decayIncrement_;
    float releaseIncrement_;
    
    float stageProgress_;
    
    void calculateIncrements() noexcept;
};

} // namespace synthcore
