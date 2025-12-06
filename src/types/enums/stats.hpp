#pragma once

#include <cstdint>

namespace types::enums {

enum class BaseStat {
    HP,
    ATK,
    DEF,
    SP_ATK,
    SP_DEF,
    SPD,

    COUNT
};

constexpr uint8_t NUM_BASE_STATS = static_cast<uint8_t>(BaseStat::COUNT);

struct EvSpread {
    uint8_t bits{0};

    constexpr EvSpread(uint8_t b = 0) : bits(b) {}

    static constexpr uint8_t mask(BaseStat s) { return 1u << static_cast<uint8_t>(s); }

    constexpr bool hp() const { return bits & mask(BaseStat::HP); }
    constexpr bool atk() const { return bits & mask(BaseStat::ATK); }
    constexpr bool def() const { return bits & mask(BaseStat::DEF); }
    constexpr bool sp_atk() const { return bits & mask(BaseStat::SP_ATK); }
    constexpr bool sp_def() const { return bits & mask(BaseStat::SP_DEF); }
    constexpr bool spd() const { return bits & mask(BaseStat::SPD); }
};

}  // namespace types::enums