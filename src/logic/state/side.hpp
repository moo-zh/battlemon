#pragma once

#include <cstdint>

namespace logic::state {

// ============================================================================
//                               SIDE STATE
// ============================================================================
//
// Domain 2: Per-team state.
// Count: 2 (one per side)
// Lifecycle: Never cleared during battle
//
// Contains:
//   - Screens (Reflect, Light Screen, Safeguard, Mist)
//   - Entry hazards (Spikes)
//   - Targeting (Follow Me)
// ============================================================================

struct SideState {
    // Screens
    uint8_t reflect_turns{0};       // 0 = inactive
    uint8_t light_screen_turns{0};
    uint8_t safeguard_turns{0};
    uint8_t mist_turns{0};

    // Entry hazards (Gen III: only Spikes)
    uint8_t spikes_layers{0};  // 0-3

    // Targeting (doubles)
    uint8_t follow_me_target{0xFF};  // 0xFF = none

    // Helpers
    constexpr bool has_reflect() const { return reflect_turns > 0; }
    constexpr bool has_light_screen() const { return light_screen_turns > 0; }
    constexpr bool has_safeguard() const { return safeguard_turns > 0; }
    constexpr bool has_mist() const { return mist_turns > 0; }
    constexpr bool has_spikes() const { return spikes_layers > 0; }

    // Reset to battle start state
    constexpr void reset() {
        reflect_turns = 0;
        light_screen_turns = 0;
        safeguard_turns = 0;
        mist_turns = 0;
        spikes_layers = 0;
        follow_me_target = 0xFF;
    }

    // Decrement screen timers (called each turn)
    constexpr void tick_screens() {
        if (reflect_turns > 0) --reflect_turns;
        if (light_screen_turns > 0) --light_screen_turns;
        if (safeguard_turns > 0) --safeguard_turns;
        if (mist_turns > 0) --mist_turns;
    }
};

}  // namespace logic::state
