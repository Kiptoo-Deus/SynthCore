#pragma once

#include "synthcore/Types.hpp"
#include "synthcore/Element.hpp"
#include "synthcore/SampleMap.hpp"
#include <array>

namespace synthcore {

class Voice {
public:
    Voice();
    ~Voice() = default;

    Voice(const Voice&) = delete;
    Voice& operator=(const Voice&) = delete;

    Voice(Voice&&) noexcept = default;
    Voice& operator=(Voice&&) noexcept = default;

    void noteOn(const SampleMap& sampleMap, MidiNote note, float velocity, SampleRate sampleRate) noexcept;
    void noteOff() noexcept;
    void reset() noexcept;
    
    Sample process(SampleRate sampleRate) noexcept;
    
    bool isActive() const noexcept;
    MidiNote getNote() const noexcept { return note_; }
    bool isNoteOn() const noexcept { return noteOn_; }
    
    Element& getElement(size_t index) noexcept { return elements_[index]; }

private:
    std::array<Element, MAX_ELEMENTS_PER_VOICE> elements_;
    MidiNote note_;
    bool noteOn_;
    bool active_;
};

} // namespace synthcore
