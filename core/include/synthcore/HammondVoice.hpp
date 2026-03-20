#pragma once

#include "synthcore/Types.hpp"
#include <array>

namespace synthcore {

class LeslieCabinet;

class HammondVoice {
public:
    HammondVoice();

    void setDrawbarLevel(int index, float level) noexcept;
    void noteOn(MidiNote note, float velocity, SampleRate sampleRate) noexcept;
    void noteOff() noexcept;
    void reset() noexcept;

    Sample process(SampleRate sampleRate, LeslieCabinet* leslie) noexcept;
    bool isActive() const noexcept;
    bool isNoteOn() const noexcept { return noteOn_; }
    MidiNote getNote() const noexcept { return note_; }

private:
    static constexpr int kDrawbarCount = 9;
    static const float kDrawbarRatios[kDrawbarCount];

    std::array<float, kDrawbarCount> drawbarLevels_;
    std::array<float, kDrawbarCount> phases_;

    MidiNote note_;
    bool noteOn_;
    bool active_;
    float velocity_;
    float baseFrequency_;
};

} // namespace synthcore