#pragma once

#include "synthcore/SampleRegion.hpp"

namespace synthcore {

struct SamplePlaybackState {
    const SampleRegion* region;
    double playbackPos;
    double playbackRatio;
    bool active;
    bool inRelease;
    bool loopActive;

    SamplePlaybackState()
        : region(nullptr)
        , playbackPos(0.0)
        , playbackRatio(1.0)
        , active(false)
        , inRelease(false)
        , loopActive(false)
    {}
};

inline float hermiteInterpolate(float p0, float p1, float p2, float p3, float t) {
    float c0 = p1;
    float c1 = 0.5f * (p2 - p0);
    float c2 = p0 - 2.5f*p1 + 2.0f*p2 - 0.5f*p3;
    float c3 = 0.5f*(p3 - p0) + 1.5f*(p1 - p2);
    return c0 + c1*t + c2*t*t + c3*t*t*t;
}

} // namespace synthcore
