#include "synthcore/Voice.hpp"

namespace synthcore {

Voice::Voice()
    : note_(0)
    , noteOn_(false)
    , active_(false)
{
}

void Voice::noteOn(const SampleMap& sampleMap, MidiNote note, float velocity, SampleRate sampleRate) noexcept {
    note_ = note;
    noteOn_ = true;
    active_ = false;
    
    const SampleRegion* region = sampleMap.lookup(note, static_cast<uint8_t>(velocity * 127.0f));
    
    if (region) {
        elements_[0].noteOn(region, note, velocity, sampleRate);
        if (elements_[0].isActive()) {
            active_ = true;
        }
    }
}

void Voice::noteOff() noexcept {
    noteOn_ = false;
    
    for (auto& element : elements_) {
        element.noteOff();
    }
}

void Voice::reset() noexcept {
    note_ = 0;
    noteOn_ = false;
    active_ = false;
    
    for (auto& element : elements_) {
        element.reset();
    }
}

Sample Voice::process(SampleRate sampleRate) noexcept {
    if (!active_) {
        return 0.0f;
    }
    
    Sample output = 0.0f;
    bool anyActive = false;
    
    for (auto& element : elements_) {
        if (element.isActive()) {
            output += element.process(sampleRate);
            anyActive = true;
        }
    }
    
    active_ = anyActive;
    
    return output;
}

bool Voice::isActive() const noexcept {
    return active_;
}

} // namespace synthcore
