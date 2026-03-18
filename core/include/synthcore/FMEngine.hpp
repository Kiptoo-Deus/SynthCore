#pragma once

#include "synthcore/Types.hpp"
#include "synthcore/FMVoice.hpp"
#include <array>

namespace synthcore {

class FMEngine {
public:
    FMEngine();
    
    void setAlgorithm(int algorithmIndex) noexcept;
    
    void setOperatorRatio(int operatorIndex, float ratio) noexcept;
    void setOperatorDetune(int operatorIndex, float detune) noexcept;
    void setOperatorLevel(int operatorIndex, float level) noexcept;
    void setOperatorEnvelope(int operatorIndex, float attack, float decay, float sustain, float release) noexcept;
    void setOperatorFeedback(int operatorIndex, float feedback) noexcept;
    
    void noteOn(MidiNote note, float velocity, SampleRate sampleRate) noexcept;
    void noteOff(MidiNote note) noexcept;
    void allNotesOff() noexcept;
    void reset() noexcept;
    
    void process(AudioBuffer& output, SampleRate sampleRate, size_t frameCount) noexcept;
    
    size_t getActiveVoiceCount() const noexcept;
    
private:
    std::array<FMVoice, MAX_VOICES> voices_;
    size_t nextVoice_;
    
    int algorithmIndex_;
    
    struct OperatorSettings {
        float ratio;
        float detune;
        float level;
        float attack;
        float decay;
        float sustain;
        float release;
        float feedback;
    };
    
    std::array<OperatorSettings, FM_OPERATORS> operatorSettings_;
    
    FMVoice* allocateVoice() noexcept;
    FMVoice* findVoiceToSteal() noexcept;
    void applySettingsToVoice(FMVoice& voice) noexcept;
};

} // namespace synthcore
