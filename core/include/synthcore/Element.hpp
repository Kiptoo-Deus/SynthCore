#pragma once

#include "synthcore/Types.hpp"
#include "synthcore/SamplePlaybackState.hpp"
#include "synthcore/EnvelopeGenerator.hpp"
#include "synthcore/LFO.hpp"
#include "synthcore/BiquadFilter.hpp"
#include "synthcore/SampleRegion.hpp"

namespace synthcore {

class Element {
public:
    Element();
    ~Element();

    void noteOn(const SampleRegion* region, MidiNote note, float velocity, SampleRate sampleRate) noexcept;
    void noteOff() noexcept;
    void reset() noexcept;
    
    Sample process(SampleRate sampleRate) noexcept;
    
    bool isActive() const noexcept;
    
    EnvelopeGenerator& envelope() noexcept { return envelope_; }
    LFO& lfo() noexcept { return lfo_; }
    BiquadFilter& filter() noexcept { return filter_; }
    
private:
    SamplePlaybackState playback_;
    EnvelopeGenerator envelope_;
    LFO lfo_;
    BiquadFilter filter_;
    
    const SampleRegion* region_;
    float velocity_;
    bool active_;
};

} // namespace synthcore
