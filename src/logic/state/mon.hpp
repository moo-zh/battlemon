#pragma once

#include <cstdint>

namespace logic::state {

// ============================================================================
//                                MON STATE
// ============================================================================
//
// Domain 4: Per-pokemon state that persists through switches.
// Count: 12 (6 per team)
// Lifecycle: Never cleared during battle (persists with the Pokemon)
//
// Contains:
//   - Current HP
//   - Primary status condition
//   - Move PP
//   - Toxic counter (resets to 1 on switch-in, not cleared)
// ============================================================================

enum class Status : uint8_t {
    NONE = 0,
    SLEEP,
    POISON,
    BURN,
    FREEZE,
    PARALYSIS,
    TOXIC,  // Badly poisoned
};

struct MonState {
    uint16_t current_hp{0};
    uint16_t max_hp{0};

    Status status{Status::NONE};
    uint8_t sleep_turns{0};    // Turns remaining for sleep
    uint8_t toxic_counter{1};  // Toxic damage multiplier (1-15)

    uint8_t pp[4]{0, 0, 0, 0};

    // Helpers
    constexpr bool is_fainted() const { return current_hp == 0; }
    constexpr bool is_alive() const { return current_hp > 0; }
    constexpr bool has_status() const { return status != Status::NONE; }
    constexpr bool is_asleep() const { return status == Status::SLEEP; }
    constexpr bool is_poisoned() const { return status == Status::POISON || status == Status::TOXIC; }
    constexpr bool is_burned() const { return status == Status::BURN; }
    constexpr bool is_frozen() const { return status == Status::FREEZE; }
    constexpr bool is_paralyzed() const { return status == Status::PARALYSIS; }

    // Apply damage (returns actual damage dealt)
    constexpr uint16_t apply_damage(uint16_t damage) {
        if (damage >= current_hp) {
            uint16_t dealt = current_hp;
            current_hp = 0;
            return dealt;
        }
        current_hp -= damage;
        return damage;
    }

    // Heal HP (returns actual HP healed)
    constexpr uint16_t heal(uint16_t amount) {
        uint16_t missing = max_hp - current_hp;
        uint16_t healed = (amount < missing) ? amount : missing;
        current_hp += healed;
        return healed;
    }

    // Reset toxic counter (called on switch-in)
    constexpr void reset_toxic_counter() {
        if (status == Status::TOXIC) {
            toxic_counter = 1;
        }
    }

    // Cure status
    constexpr void cure_status() {
        status = Status::NONE;
        sleep_turns = 0;
        toxic_counter = 1;
    }
};

}  // namespace logic::state
