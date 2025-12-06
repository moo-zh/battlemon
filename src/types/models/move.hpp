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
            MAKES_CONTACT,
            PROTECT_AFFECTED,
            MAGIC_COAT_AFFECTED,
            SNATCH_AFFECTED,
            MIRROR_MOVE_AFFECTED,
            KINGS_ROCK_AFFECTED,
            RESERVED_1,
            RESERVED_2
        };

        static constexpr uint8_t mask(Bit b) { return 1u << static_cast<uint8_t>(b); }

        constexpr bool makes_contact() const { return bits & mask(MAKES_CONTACT); }
        constexpr bool protect_affected() const { return bits & mask(PROTECT_AFFECTED); }
        constexpr bool magic_coat_affected() const { return bits & mask(MAGIC_COAT_AFFECTED); }
        constexpr bool snatch_affected() const { return bits & mask(SNATCH_AFFECTED); }
        constexpr bool mirror_move_affected() const { return bits & mask(MIRROR_MOVE_AFFECTED); }
        constexpr bool kings_rock_affected() const { return bits & mask(KINGS_ROCK_AFFECTED); }
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