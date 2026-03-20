#include "synthcore/HammondVoice.hpp"
#include "synthcore/LeslieCabinet.hpp"
#include <cmath>

namespace synthcore {

const float HammondVoice::kDrawbarRatios[HammondVoice::kDrawbarCount] = {
    1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 8.0f, 10.0f, 12.0f
};

HammondVoice::HammondVoice()
    : note_(0)
    , noteOn_(false)
    , active_(false)
    , velocity_(0.0f)
    , baseFrequency_(440.0f)
{
    drawbarLevels_.fill(0.0f);
    phases_.fill(0.0f);
}

void HammondVoice::setDrawbarLevel(int index, float level) noexcept {
    if (index >= 0 && index < kDrawbarCount) {
        float clamped = level < 0.0f ? 0.0f : (level > 1.0f ? 1.0f : level);
        drawbarLevels_[index] = clamped;
    }
}

void HammondVoice::noteOn(MidiNote note, float velocity, SampleRate sampleRate) noexcept {
    (void)sampleRate;
    note_ = note;
    noteOn_ = true;
    active_ = true;
    velocity_ = velocity;
    baseFrequency_ = static_cast<float>(A4_FREQUENCY * std::pow(2.0, (note - 69) / 12.0));
    phases_.fill(0.0f);
}

void HammondVoice::noteOff() noexcept {
    noteOn_ = false;
}

void HammondVoice::reset() noexcept {
    note_ = 0;
    noteOn_ = false;
    active_ = false;
    velocity_ = 0.0f;
    baseFrequency_ = 440.0f;
    drawbarLevels_.fill(0.0f);
    phases_.fill(0.0f);
}

Sample HammondVoice::process(SampleRate sampleRate, LeslieCabinet* leslie) noexcept {
    if (!active_) {
        return 0.0f;
    }

    float output = 0.0f;
    for (int i = 0; i < kDrawbarCount; ++i) {
        float ratio = kDrawbarRatios[i];
        float env = drawbarLevels_[i];
        if (env <= 0.0f) {
            continue;
        }

        float freq = baseFrequency_ * ratio;
        float phaseInc = freq / static_cast<float>(sampleRate);
        phases_[i] += phaseInc;
        if (phases_[i] >= 1.0f) {
            phases_[i] -= 1.0f;
        }

        output += env * std::sin(phases_[i] * 2.0f * 3.14159265358979323846f);
    }

    output *= velocity_; 
    if (leslie) {
        output = leslie->process(output, sampleRate);
    }

    if (!noteOn_) {
        output *= 0.995f;
        if (std::abs(output) < 1e-5f) {
            active_ = false;
        }
    }

    return output;
}

bool HammondVoice::isActive() const noexcept {
    return active_;
}

} // namespace synthcore