#pragma once

#include <cstdint>
#include <array>

namespace synthcore {

constexpr int FM_OPERATORS = 8;
constexpr int FM_ALGORITHM_COUNT = 88;

struct FMConnection {
    uint8_t source;
    uint8_t destination;
};

struct FMAlgorithmDef {
    FMConnection connections[16];
    uint8_t carriers[8];
    uint8_t connectionCount;
    uint8_t carrierCount;
};

class FMAlgorithm {
public:
    static const FMAlgorithmDef& getAlgorithm(int algorithmIndex) noexcept;
    static bool isCarrier(int algorithmIndex, int operatorIndex) noexcept;
    
private:
    static const std::array<FMAlgorithmDef, FM_ALGORITHM_COUNT> algorithms_;
};

} // namespace synthcore
