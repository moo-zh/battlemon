#pragma once

#include <cstdint>

namespace types {

struct EvSpread {
    enum Bit : uint8_t {
        HP = 1 << 0,
        ATK = 1 << 1,
        DEF = 1 << 2,
        SP_ATK = 1 << 3,
        SP_DEF = 1 << 4,
        SPD = 1 << 5,
    };

    uint8_t bits{0};

    constexpr EvSpread(uint8_t b = 0) : bits(b) {}

    constexpr bool hp() const { return bits & HP; }
    constexpr bool atk() const { return bits & ATK; }
    constexpr bool def() const { return bits & DEF; }
    constexpr bool sp_atk() const { return bits & SP_ATK; }
    constexpr bool sp_def() const { return bits & SP_DEF; }
    constexpr bool spd() const { return bits & SPD; }
};

}  // namespace types