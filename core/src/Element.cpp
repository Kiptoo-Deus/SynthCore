#include "synthcore/Element.hpp"
#include <cmath>

namespace synthcore {

Element::Element()
    : region_(nullptr)
    , velocity_(0.0f)
    , active_(false)
{
}

Element::~Element() = default;

void Element::noteOn(const SampleRegion* region, MidiNote note, float velocity, SampleRate sampleRate) noexcept {
    if (!region || region->samples.empty()) {
        active_ = false;
        return;
    }
    
    region_ = region;
    velocity_ = velocity;
    active_ = true;
    
    float semitoneOffset = static_cast<float>(note) - static_cast<float>(region->rootNote);
    playback_.playbackRatio = std::pow(2.0, semitoneOffset / 12.0);
    playback_.playbackPos = 0.0;
    playback_.loopActive = (region->loopStart < region->loopEnd && 
                            region->loopEnd < region->samples.size());
    playback_.region = region;
    playback_.active = true;
    playback_.inRelease = false;
    
    envelope_.noteOn(static_cast<float>(sampleRate));
    lfo_.reset();
    filter_.reset();
}

void Element::noteOff() noexcept {
    envelope_.noteOff();
}

void Element::reset() noexcept {
    region_ = nullptr;
    velocity_ = 0.0f;
    active_ = false;
    playback_.playbackPos = 0.0;
    playback_.playbackRatio = 1.0;
    playback_.region = nullptr;
    playback_.active = false;
    playback_.loopActive = false;
    playback_.inRelease = false;
    lfo_.reset();
    filter_.reset();
}

Sample Element::process(SampleRate) noexcept {
    if (!active_ || !region_) {
        return 0.0f;
    }
    
    size_t idx = static_cast<size_t>(playback_.playbackPos);
    if (idx >= region_->samples.size() - 4) {
        if (!playback_.loopActive || region_->loopStart >= region_->loopEnd) {
            active_ = false;
            return 0.0f;
        }
        idx = region_->loopStart;
        playback_.playbackPos = static_cast<double>(region_->loopStart);
    }
    
    float t = static_cast<float>(playback_.playbackPos - idx);
    Sample output = hermiteInterpolate(
        region_->samples[idx],
        region_->samples[idx + 1],
        region_->samples[idx + 2],
        region_->samples[idx + 3],
        t
    );
    
    float envValue = envelope_.process();
    if (envelope_.isIdle()) {
        active_ = false;
        return 0.0f;
    }
    
    output *= envValue * velocity_;
    
    output = filter_.process(output);
    
    playback_.playbackPos += playback_.playbackRatio;
    
    if (playback_.loopActive && playback_.playbackPos >= static_cast<double>(region_->loopEnd)) {
        playback_.playbackPos = static_cast<double>(region_->loopStart) + 
            std::fmod(playback_.playbackPos - static_cast<double>(region_->loopEnd), 
                     static_cast<double>(region_->loopEnd - region_->loopStart));
    }
    
    return output;
}

bool Element::isActive() const noexcept {
    return active_;
}

} // namespace synthcore
