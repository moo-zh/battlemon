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
        bool makes_contact : 1;
        bool protect_affected : 1;
        bool magic_coat_affected : 1;
        bool snatch_affected : 1;
        bool mirror_move_affected : 1;
        bool kings_rock_affected : 1;
        bool reserved_1 : 1;
        bool reserved_2 : 1;

        constexpr Flags(uint8_t bits = 0)
            : makes_contact(bits & 0x01)
            , protect_affected(bits & 0x02)
            , magic_coat_affected(bits & 0x04)
            , snatch_affected(bits & 0x08)
            , mirror_move_affected(bits & 0x10)
            , kings_rock_affected(bits & 0x20)
            , reserved_1(bits & 0x40)
            , reserved_2(bits & 0x80) {}
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