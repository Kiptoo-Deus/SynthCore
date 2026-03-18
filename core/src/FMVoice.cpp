#include "synthcore/FMVoice.hpp"
#include <cmath>
#include <algorithm>

namespace synthcore {

FMVoice::FMVoice()
    : algorithmIndex_(0)
    , note_(0)
    , noteOn_(false)
    , active_(false)
{
    operatorOutputs_.fill(0.0f);
}

void FMVoice::setAlgorithm(int algorithmIndex) noexcept {
    algorithmIndex_ = std::clamp(algorithmIndex, 0, FM_ALGORITHM_COUNT - 1);
}

void FMVoice::setOperatorRatio(int operatorIndex, float ratio) noexcept {
    if (operatorIndex >= 0 && operatorIndex < FM_OPERATORS) {
        operators_[operatorIndex].setFrequencyRatio(ratio);
    }
}

void FMVoice::setOperatorDetune(int operatorIndex, float detune) noexcept {
    if (operatorIndex >= 0 && operatorIndex < FM_OPERATORS) {
        operators_[operatorIndex].setDetune(detune);
    }
}

void FMVoice::setOperatorLevel(int operatorIndex, float level) noexcept {
    if (operatorIndex >= 0 && operatorIndex < FM_OPERATORS) {
        operators_[operatorIndex].setOutputLevel(level);
    }
}

void FMVoice::setOperatorEnvelope(int operatorIndex, float attack, float decay, float sustain, float release) noexcept {
    if (operatorIndex >= 0 && operatorIndex < FM_OPERATORS) {
        operators_[operatorIndex].setEnvelope(attack, decay, sustain, release);
    }
}

void FMVoice::setOperatorFeedback(int operatorIndex, float feedback) noexcept {
    if (operatorIndex >= 0 && operatorIndex < FM_OPERATORS) {
        operators_[operatorIndex].setFeedback(feedback);
    }
}

void FMVoice::noteOn(MidiNote note, float velocity, SampleRate sampleRate) noexcept {
    note_ = note;
    noteOn_ = true;
    active_ = true;
    
    float baseFrequency = midiNoteToFrequency(note);
    
    for (auto& op : operators_) {
        op.noteOn(baseFrequency, velocity, sampleRate);
    }
    
    operatorOutputs_.fill(0.0f);
}

void FMVoice::noteOff() noexcept {
    noteOn_ = false;
    
    for (auto& op : operators_) {
        op.noteOff();
    }
}

void FMVoice::reset() noexcept {
    note_ = 0;
    noteOn_ = false;
    active_ = false;
    
    for (auto& op : operators_) {
        op.reset();
    }
    
    operatorOutputs_.fill(0.0f);
}

Sample FMVoice::process(SampleRate sampleRate) noexcept {
    if (!active_) {
        return 0.0f;
    }
    
    const auto& algorithm = FMAlgorithm::getAlgorithm(algorithmIndex_);
    
    operatorOutputs_.fill(0.0f);
    
    for (int i = 0; i < FM_OPERATORS; ++i) {
        Sample modulation = 0.0f;
        
        for (uint8_t c = 0; c < algorithm.connectionCount; ++c) {
            if (algorithm.connections[c].destination == i) {
                modulation += operatorOutputs_[algorithm.connections[c].source];
            }
        }
        
        operatorOutputs_[i] = operators_[i].process(modulation, sampleRate);
    }
    
    Sample output = 0.0f;
    for (uint8_t c = 0; c < algorithm.carrierCount; ++c) {
        output += operatorOutputs_[algorithm.carriers[c]];
    }
    
    bool anyActive = false;
    for (const auto& op : operators_) {
        if (op.isActive()) {
            anyActive = true;
            break;
        }
    }
    active_ = anyActive;
    
    return output / static_cast<float>(algorithm.carrierCount > 0 ? algorithm.carrierCount : 1);
}

bool FMVoice::isActive() const noexcept {
    return active_;
}

float FMVoice::midiNoteToFrequency(MidiNote note) const noexcept {
    return static_cast<float>(A4_FREQUENCY * std::pow(2.0, (note - 69) / 12.0));
}

} // namespace synthcore
