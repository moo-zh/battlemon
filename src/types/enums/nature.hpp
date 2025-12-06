#pragma once

#include <cstdint>

namespace types::enums {

enum class Nature : uint8_t {
    // clang-format off

    ADAMANT, BASHFUL, BOLD,    BRAVE,   CALM,
    CAREFUL, DOCILE,  GENTLE,  HARDY,   HASTY,
    IMPISH,  JOLLY,   LAX,     LONELY,  MILD,
    MODEST,  NAIVE,   NAUGHTY, QUIET,   QUIRKY,
    RASH,    RELAXED, SASSY,   SERIOUS, TIMID,

    // clang-format on
};

}  // namespace types::enums
