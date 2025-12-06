#pragma once

#include <cstdint>

namespace types::enums {

enum class Type : uint8_t {
    // clang-format off
    
    NONE,

    NORMAL,   FIGHTING, FLYING, POISON,
    GROUND,   ROCK,     BUG,    GHOST,    
    STEEL,    FIRE,     WATER,  GRASS,    
    ELECTRIC, PSYCHIC,  ICE,    DRAGON,   
    DARK,

    // clang-format on
};

}  // namespace types::enums