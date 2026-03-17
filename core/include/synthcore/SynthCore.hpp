#pragma once

#include "synthcore/Types.hpp"

namespace synthcore {

class SynthCore {
public:
    SynthCore();
    ~SynthCore();

    SynthCore(const SynthCore&) = delete;
    SynthCore& operator=(const SynthCore&) = delete;

    SynthCore(SynthCore&&) noexcept = default;
    SynthCore& operator=(SynthCore&&) noexcept = default;

private:
};

} // namespace synthcore
