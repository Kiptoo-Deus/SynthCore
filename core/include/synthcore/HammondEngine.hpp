#pragma once

#include "synthcore/Types.hpp"
#include "synthcore/HammondVoice.hpp"
#include "synthcore/LeslieCabinet.hpp"
#include <array>

namespace synthcore {

class HammondEngine {
public:
    HammondEngine();

    void setDrawbar(int index, float level) noexcept;
    void setLeslieRate(float rateHz) noexcept;
    void setLeslieDepth(float depth) noexcept;

    void noteOn(MidiNote note, float velocity, SampleRate sampleRate) noexcept;
    void noteOff(MidiNote note) noexcept;
    void allNotesOff() noexcept;
    void reset() noexcept;

    void process(AudioBuffer& output, SampleRate sampleRate, size_t frameCount) noexcept;
    size_t getActiveVoiceCount() const noexcept;

private:
    std::array<HammondVoice, MAX_VOICES> voices_;
    size_t nextVoice_;
    LeslieCabinet leslie_;

    HammondVoice* allocateVoice() noexcept;
    HammondVoice* findVoiceToSteal() noexcept;
};

} // namespace synthcore