#pragma once

#include "synthcore/Types.hpp"
#include "synthcore/Voice.hpp"
#include "synthcore/SampleMap.hpp"
#include <array>

namespace synthcore {

class VoiceManager {
public:
    VoiceManager();
    ~VoiceManager() = default;

    void noteOn(const SampleMap& sampleMap, MidiNote note, float velocity, SampleRate sampleRate) noexcept;
    void noteOff(MidiNote note) noexcept;
    void allNotesOff() noexcept;
    void reset() noexcept;
    
    void process(AudioBuffer& output, const SampleMap& sampleMap, SampleRate sampleRate, size_t frameCount) noexcept;
    
    size_t getActiveVoiceCount() const noexcept;
    
private:
    Voice* allocateVoice() noexcept;
    Voice* findVoiceToSteal() noexcept;
    
    std::array<Voice, MAX_VOICES> voices_;
    size_t nextVoice_;
};

} // namespace synthcore
