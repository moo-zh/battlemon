#pragma once

#include <cstdint>

namespace logic::state {

// ============================================================================
//                              FIELD STATE
// ============================================================================
//
// Domain 1: Global state affecting all battlers.
// Count: 1
// Lifecycle: Never cleared during battle
//
// Contains:
//   - Weather (type and duration)
//   - Future Sight / Doom Desire tracking
//   - Wish tracking
// ============================================================================

enum class Weather : uint8_t {
    NONE = 0,
    SUN,
    RAIN,
    SANDSTORM,
    HAIL,
};

struct FutureSight {
    uint8_t counter[4];       // Turns until attack lands (0 = inactive)
    uint8_t attacker[4];      // Slot that used it
    uint16_t damage[4];       // Pre-calculated damage
    uint8_t move[4];          // Move ID (FUTURE_SIGHT or DOOM_DESIRE)
};

struct Wish {
    uint8_t counter[4];       // Turns until heal (0 = inactive)
    uint8_t hp_to_restore[4]; // HP amount to restore
};

struct FieldState {
    Weather weather{Weather::NONE};
    uint8_t weather_turns{0};  // 0 = permanent (ability-induced)

    FutureSight future_sight{};
    Wish wish{};

    // Reset to battle start state
    constexpr void reset() {
        weather = Weather::NONE;
        weather_turns = 0;
        future_sight = {};
        wish = {};
    }
};

}  // namespace logic::state
