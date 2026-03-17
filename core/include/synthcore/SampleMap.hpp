#pragma once

#include "synthcore/SampleRegion.hpp"
#include <array>
#include <vector>
#include <memory>

namespace synthcore {

class SampleMap {
public:
    SampleMap();
    ~SampleMap();

    void addRegion(SampleRegion region);
    const SampleRegion* lookup(uint8_t midiNote, uint8_t velocity) const noexcept;
    int getRegionCount() const noexcept;
    void clear();

private:
    std::vector<std::unique_ptr<SampleRegion>> regions_;
    std::array<std::vector<SampleRegion*>, 128> noteMap_;
    mutable std::array<int, 128> rrCounters_;
};

} // namespace synthcore
