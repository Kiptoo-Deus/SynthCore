#pragma once

#include "synthcore/Types.hpp"

namespace synthcore {

class Voice {
public:
    Voice() = default;
    ~Voice() = default;

    Voice(const Voice&) = delete;
    Voice& operator=(const Voice&) = delete;

    Voice(Voice&&) noexcept = default;
    Voice& operator=(Voice&&) noexcept = default;

private:
};

} // namespace synthcore
