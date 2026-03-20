#include "synthcore/HammondEngine.hpp"
#include <algorithm>

namespace synthcore {

HammondEngine::HammondEngine()
    : nextVoice_(0)
{
}

void HammondEngine::setDrawbar(int index, float level) noexcept {
    for (auto& voice : voices_) {
        voice.setDrawbarLevel(index, level);
    }
}

void HammondEngine::setLeslieRate(float rateHz) noexcept {
    leslie_.setRate(rateHz);
}

void HammondEngine::setLeslieDepth(float depth) noexcept {
    leslie_.setDepth(depth);
}

void HammondEngine::noteOn(MidiNote note, float velocity, SampleRate sampleRate) noexcept {
    HammondVoice* voice = allocateVoice();
    if (!voice) {
        voice = findVoiceToSteal();
    }
    if (voice) {
        voice->noteOn(note, velocity, sampleRate);
    }
}

void HammondEngine::noteOff(MidiNote note) noexcept {
    for (auto& voice : voices_) {
        if (voice.isNoteOn() && voice.getNote() == note) {
            voice.noteOff();
        }
    }
}

void HammondEngine::allNotesOff() noexcept {
    for (auto& voice : voices_) {
        voice.noteOff();
    }
}

void HammondEngine::reset() noexcept {
    for (auto& voice : voices_) {
        voice.reset();
    }
    nextVoice_ = 0;
    leslie_.reset();
}

void HammondEngine::process(AudioBuffer& output, SampleRate sampleRate, size_t frameCount) noexcept {
    if (frameCount > static_cast<size_t>(std::size(output.data))) {
        frameCount = static_cast<size_t>(std::size(output.data));
    }

    std::fill(std::begin(output.data), std::begin(output.data) + frameCount, 0.0f);

    for (auto& voice : voices_) {
        if (voice.isActive()) {
            for (size_t i = 0; i < frameCount; ++i) {
                output.data[i] += voice.process(sampleRate, &leslie_);
            }
        }
    }

    output.size = static_cast<size_t>(frameCount);
}

size_t HammondEngine::getActiveVoiceCount() const noexcept {
    size_t count = 0;
    for (const auto& voice : voices_) {
        if (voice.isActive()) {
            ++count;
        }
    }
    return count;
}

HammondVoice* HammondEngine::allocateVoice() noexcept {
    for (auto& voice : voices_) {
        if (!voice.isActive()) {
            return &voice;
        }
    }
    return nullptr;
}

HammondVoice* HammondEngine::findVoiceToSteal() noexcept {
    HammondVoice* oldest = nullptr;
    for (auto& voice : voices_) {
        if (!voice.isNoteOn()) {
            oldest = &voice;
            break;
        }
    }
    if (!oldest) {
        oldest = &voices_[nextVoice_];
        nextVoice_ = (nextVoice_ + 1) % MAX_VOICES;
    }
    return oldest;
}

} // namespace synthcore