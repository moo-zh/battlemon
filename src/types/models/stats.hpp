#pragma once

#include <cstdint>

#include "../enums/stats.hpp"

namespace types {

struct EvSpread {
    uint8_t bits{0};

    constexpr EvSpread(uint8_t b = 0) : bits(b) {}

    static constexpr uint8_t mask(enums::BaseStat s) { return 1u << static_cast<uint8_t>(s); }

    constexpr bool hp() const { return bits & mask(enums::BaseStat::HP); }
    constexpr bool atk() const { return bits & mask(enums::BaseStat::ATK); }
    constexpr bool def() const { return bits & mask(enums::BaseStat::DEF); }
    constexpr bool sp_atk() const { return bits & mask(enums::BaseStat::SP_ATK); }
    constexpr bool sp_def() const { return bits & mask(enums::BaseStat::SP_DEF); }
    constexpr bool spd() const { return bits & mask(enums::BaseStat::SPD); }
};

}  // namespace types