#include "synthcore/FMEngine.hpp"
#include <algorithm>

namespace synthcore {

FMEngine::FMEngine()
    : nextVoice_(0)
    , algorithmIndex_(0)
{
    for (auto& settings : operatorSettings_) {
        settings.ratio = 1.0f;
        settings.detune = 0.0f;
        settings.level = 1.0f;
        settings.attack = 0.01f;
        settings.decay = 0.1f;
        settings.sustain = 0.7f;
        settings.release = 0.2f;
        settings.feedback = 0.0f;
    }
}

void FMEngine::setAlgorithm(int algorithmIndex) noexcept {
    algorithmIndex_ = std::clamp(algorithmIndex, 0, FM_ALGORITHM_COUNT - 1);
}

void FMEngine::setOperatorRatio(int operatorIndex, float ratio) noexcept {
    if (operatorIndex >= 0 && operatorIndex < FM_OPERATORS) {
        operatorSettings_[operatorIndex].ratio = ratio;
    }
}

void FMEngine::setOperatorDetune(int operatorIndex, float detune) noexcept {
    if (operatorIndex >= 0 && operatorIndex < FM_OPERATORS) {
        operatorSettings_[operatorIndex].detune = detune;
    }
}

void FMEngine::setOperatorLevel(int operatorIndex, float level) noexcept {
    if (operatorIndex >= 0 && operatorIndex < FM_OPERATORS) {
        operatorSettings_[operatorIndex].level = level;
    }
}

void FMEngine::setOperatorEnvelope(int operatorIndex, float attack, float decay, float sustain, float release) noexcept {
    if (operatorIndex >= 0 && operatorIndex < FM_OPERATORS) {
        operatorSettings_[operatorIndex].attack = attack;
        operatorSettings_[operatorIndex].decay = decay;
        operatorSettings_[operatorIndex].sustain = sustain;
        operatorSettings_[operatorIndex].release = release;
    }
}

void FMEngine::setOperatorFeedback(int operatorIndex, float feedback) noexcept {
    if (operatorIndex >= 0 && operatorIndex < FM_OPERATORS) {
        operatorSettings_[operatorIndex].feedback = feedback;
    }
}

void FMEngine::noteOn(MidiNote note, float velocity, SampleRate sampleRate) noexcept {
    FMVoice* voice = allocateVoice();
    if (!voice) {
        voice = findVoiceToSteal();
    }
    
    if (voice) {
        applySettingsToVoice(*voice);
        voice->noteOn(note, velocity, sampleRate);
    }
}

void FMEngine::noteOff(MidiNote note) noexcept {
    for (auto& voice : voices_) {
        if (voice.isNoteOn() && voice.getNote() == note) {
            voice.noteOff();
        }
    }
}

void FMEngine::allNotesOff() noexcept {
    for (auto& voice : voices_) {
        voice.noteOff();
    }
}

void FMEngine::reset() noexcept {
    for (auto& voice : voices_) {
        voice.reset();
    }
    nextVoice_ = 0;
}

void FMEngine::process(AudioBuffer& output, SampleRate sampleRate, size_t frameCount) noexcept {
    std::fill(output.data, output.data + frameCount, 0.0f);
    
    for (auto& voice : voices_) {
        if (voice.isActive()) {
            for (size_t i = 0; i < frameCount; ++i) {
                output.data[i] += voice.process(sampleRate);
            }
        }
    }
}

size_t FMEngine::getActiveVoiceCount() const noexcept {
    size_t count = 0;
    for (const auto& voice : voices_) {
        if (voice.isActive()) {
            ++count;
        }
    }
    return count;
}

FMVoice* FMEngine::allocateVoice() noexcept {
    for (auto& voice : voices_) {
        if (!voice.isActive()) {
            return &voice;
        }
    }
    return nullptr;
}

FMVoice* FMEngine::findVoiceToSteal() noexcept {
    FMVoice* oldest = nullptr;
    
    for (auto& voice : voices_) {
        if (!voice.isNoteOn()) {
            if (!oldest) {
                oldest = &voice;
            }
        }
    }
    
    if (!oldest) {
        oldest = &voices_[nextVoice_];
        nextVoice_ = (nextVoice_ + 1) % MAX_VOICES;
    }
    
    return oldest;
}

void FMEngine::applySettingsToVoice(FMVoice& voice) noexcept {
    voice.setAlgorithm(algorithmIndex_);
    
    for (int i = 0; i < FM_OPERATORS; ++i) {
        const auto& settings = operatorSettings_[i];
        voice.setOperatorRatio(i, settings.ratio);
        voice.setOperatorDetune(i, settings.detune);
        voice.setOperatorLevel(i, settings.level);
        voice.setOperatorEnvelope(i, settings.attack, settings.decay, settings.sustain, settings.release);
        voice.setOperatorFeedback(i, settings.feedback);
    }
}

} // namespace synthcore
