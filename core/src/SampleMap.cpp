#include "synthcore/SampleMap.hpp"

namespace synthcore {

SampleMap::SampleMap() {
    rrCounters_.fill(0);
}

SampleMap::~SampleMap() {
}

void SampleMap::addRegion(SampleRegion region) {
    auto regionPtr = std::make_unique<SampleRegion>(std::move(region));
    SampleRegion* rawPtr = regionPtr.get();
    
    for (uint8_t note = rawPtr->loNote; note <= rawPtr->hiNote; ++note) {
        noteMap_[note].push_back(rawPtr);
    }
    
    regions_.push_back(std::move(regionPtr));
}

const SampleRegion* SampleMap::lookup(uint8_t midiNote, uint8_t velocity) const noexcept {
    if (midiNote > 127) return nullptr;
    
    const auto& noteRegions = noteMap_[midiNote];
    if (noteRegions.empty()) return nullptr;
    
    std::vector<const SampleRegion*> matchingRegions;
    
    for (const auto* region : noteRegions) {
        if (velocity >= region->loVel && velocity <= region->hiVel) {
            matchingRegions.push_back(region);
        }
    }
    
    if (matchingRegions.empty()) return nullptr;
    
    if (matchingRegions.size() == 1) {
        return matchingRegions[0];
    }
    
    std::vector<const SampleRegion*> rrRegions;
    for (const auto* region : matchingRegions) {
        if (region->rrGroup >= 0) {
            rrRegions.push_back(region);
        }
    }
    
    if (!rrRegions.empty()) {
        int& counter = rrCounters_[midiNote];
        const SampleRegion* selected = rrRegions[counter % rrRegions.size()];
        counter++;
        return selected;
    }
    
    return matchingRegions[0];
}

int SampleMap::getRegionCount() const noexcept {
    return static_cast<int>(regions_.size());
}

void SampleMap::clear() {
    regions_.clear();
    for (auto& noteRegions : noteMap_) {
        noteRegions.clear();
    }
    rrCounters_.fill(0);
}

} // namespace synthcore
