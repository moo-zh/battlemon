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

}  // namespace types::enums