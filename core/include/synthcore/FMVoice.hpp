#pragma once

#include "synthcore/Types.hpp"
#include "synthcore/FMOperator.hpp"
#include "synthcore/FMAlgorithm.hpp"
#include <array>

namespace synthcore {

class FMVoice {
public:
    FMVoice();
    
    void setAlgorithm(int algorithmIndex) noexcept;
    
    void setOperatorRatio(int operatorIndex, float ratio) noexcept;
    void setOperatorDetune(int operatorIndex, float detune) noexcept;
    void setOperatorLevel(int operatorIndex, float level) noexcept;
    void setOperatorEnvelope(int operatorIndex, float attack, float decay, float sustain, float release) noexcept;
    void setOperatorFeedback(int operatorIndex, float feedback) noexcept;
    
    void noteOn(MidiNote note, float velocity, SampleRate sampleRate) noexcept;
    void noteOff() noexcept;
    void reset() noexcept;
    
    Sample process(SampleRate sampleRate) noexcept;
    
    bool isActive() const noexcept;
    MidiNote getNote() const noexcept { return note_; }
    bool isNoteOn() const noexcept { return noteOn_; }
    
private:
    std::array<FMOperator, FM_OPERATORS> operators_;
    std::array<Sample, FM_OPERATORS> operatorOutputs_;
    
    int algorithmIndex_;
    MidiNote note_;
    bool noteOn_;
    bool active_;
    
    float midiNoteToFrequency(MidiNote note) const noexcept;
};

} // namespace synthcore
