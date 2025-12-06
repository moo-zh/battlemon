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
    bool hp     : 1;
    bool atk    : 1;
    bool def    : 1;
    bool sp_atk : 1;
    bool sp_def : 1;
    bool spd    : 1;
    bool : 2;  // padding

    constexpr EvSpread(uint8_t bits = 0)
        : hp(bits & 0x01)
        , atk(bits & 0x02)
        , def(bits & 0x04)
        , sp_atk(bits & 0x08)
        , sp_def(bits & 0x10)
        , spd(bits & 0x20) {}
};

}  // namespace types::enums