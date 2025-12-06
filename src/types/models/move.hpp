#pragma once

#include <cstdint>

#include "../enums/effect.hpp"
#include "../enums/move.hpp"
#include "../enums/type.hpp"

namespace types {

struct Move {
    // ============================================================
    //                          Move Flags
    // ============================================================

    struct Flags {
        uint8_t bits{0};

        constexpr Flags(uint8_t b = 0) : bits(b) {}

        enum Bit : uint8_t {
            MAKES_CONTACT = 1 << 0,
            PROTECT_AFFECTED = 1 << 1,
            MAGIC_COAT_AFFECTED = 1 << 2,
            SNATCH_AFFECTED = 1 << 3,
            MIRROR_MOVE_AFFECTED = 1 << 4,
            KINGS_ROCK_AFFECTED = 1 << 5,
            RESERVED_1 = 1 << 6,
            RESERVED_2 = 1 << 7
        };

        constexpr bool makes_contact() const { return bits & MAKES_CONTACT; }
        constexpr bool protect_affected() const { return bits & PROTECT_AFFECTED; }
        constexpr bool magic_coat_affected() const { return bits & MAGIC_COAT_AFFECTED; }
        constexpr bool snatch_affected() const { return bits & SNATCH_AFFECTED; }
        constexpr bool mirror_move_affected() const { return bits & MIRROR_MOVE_AFFECTED; }
        constexpr bool kings_rock_affected() const { return bits & KINGS_ROCK_AFFECTED; }
    };

    // ============================================================
    //                         Move Target
    // ============================================================

    enum class Target : uint8_t {
        SELECTED,
        DEPENDS,
        USER,
        RANDOM,
        BOTH,
        OPPONENTS_FIELD,
        USER_OR_ALLY,
        OPPONENTS_FIELD_2,
        FOES_AND_ALLY,
        USER_AND_ALLIES
    };

    // ============================================================
    //                          Move Data
    // ============================================================

    enums::Move id;
    enums::Type type;

    uint8_t power;
    uint8_t accuracy;
    uint8_t pp;
    int8_t priority;

    enums::Effect effect;
    uint8_t effect_chance;
    Target target;
    Flags flags;
};

using MoveFlags = Move::Flags;
using MoveTarget = Move::Target;

}  // namespace types