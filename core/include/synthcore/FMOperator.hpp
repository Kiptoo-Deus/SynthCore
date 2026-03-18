#pragma once

#include "synthcore/Types.hpp"
#include <cstdint>

namespace synthcore {

class FMOperator {
public:
    FMOperator();
    
    void setFrequencyRatio(float ratio) noexcept { frequencyRatio_ = ratio; }
    void setDetune(float detune) noexcept { detune_ = detune; }
    void setOutputLevel(float level) noexcept { outputLevel_ = level; }
    void setFeedback(float feedback) noexcept { feedback_ = feedback; }
    
    void setEnvelope(float attack, float decay, float sustain, float release) noexcept;
    
    void noteOn(float baseFrequency, float velocity, SampleRate sampleRate) noexcept;
    void noteOff() noexcept;
    void reset() noexcept;
    
    Sample process(Sample modulation, SampleRate sampleRate) noexcept;
    
    bool isActive() const noexcept;
    
private:
    float phase_;
    float frequencyRatio_;
    float detune_;
    float baseFrequency_;
    float outputLevel_;
    float feedback_;
    float feedbackSample_;
    
    float envelopeLevel_;
    float velocity_;
    
    enum class EnvStage { Idle, Attack, Decay, Sustain, Release };
    EnvStage envStage_;
    
    float attackRate_;
    float decayRate_;
    float sustainLevel_;
    float releaseRate_;
    
    float stageProgress_;
    SampleRate sampleRate_;
    
    void processEnvelope() noexcept;
    float calculateIncrement() const noexcept;
};

} // namespace synthcore
