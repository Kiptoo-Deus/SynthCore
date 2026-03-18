#include "synthcore/VoiceManager.hpp"
#include <algorithm>

namespace synthcore {

VoiceManager::VoiceManager()
    : nextVoice_(0)
{
}

void VoiceManager::noteOn(const SampleMap& sampleMap, MidiNote note, float velocity, SampleRate sampleRate) noexcept {
    Voice* voice = allocateVoice();
    if (!voice) {
        voice = findVoiceToSteal();
    }
    
    if (voice) {
        voice->noteOn(sampleMap, note, velocity, sampleRate);
    }
}

void VoiceManager::noteOff(MidiNote note) noexcept {
    for (auto& voice : voices_) {
        if (voice.isNoteOn() && voice.getNote() == note) {
            voice.noteOff();
        }
    }
}

void VoiceManager::allNotesOff() noexcept {
    for (auto& voice : voices_) {
        voice.noteOff();
    }
}

void VoiceManager::reset() noexcept {
    for (auto& voice : voices_) {
        voice.reset();
    }
    nextVoice_ = 0;
}

void VoiceManager::process(AudioBuffer& output, const SampleMap&, SampleRate sampleRate, size_t frameCount) noexcept {
    std::fill(output.data, output.data + frameCount, 0.0f);
    
    for (auto& voice : voices_) {
        if (voice.isActive()) {
            for (size_t i = 0; i < frameCount; ++i) {
                output.data[i] += voice.process(sampleRate);
            }
        }
    }
}

size_t VoiceManager::getActiveVoiceCount() const noexcept {
    size_t count = 0;
    for (const auto& voice : voices_) {
        if (voice.isActive()) {
            ++count;
        }
    }
    return count;
}

Voice* VoiceManager::allocateVoice() noexcept {
    for (auto& voice : voices_) {
        if (!voice.isActive()) {
            return &voice;
        }
    }
    return nullptr;
}

Voice* VoiceManager::findVoiceToSteal() noexcept {
    Voice* oldest = nullptr;
    
    for (auto& voice : voices_) {
        if (!voice.isNoteOn()) {
            if (!oldest || true) {
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

} // namespace synthcore
