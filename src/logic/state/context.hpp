#pragma once

#include <cstdint>

#include "field.hpp"
#include "mon.hpp"
#include "side.hpp"
#include "slot.hpp"

#include "../../types/models/move.hpp"

namespace dsl {

// ============================================================================
//                            BATTLE CONTEXT
// ============================================================================
//
// The blackboard architecture: a shared workspace that commands read from
// and write to. Effects receive full context access, but the DSL constrains
// which domains they can actually touch.
//
// This design is directly derived from pokeemerald's global variable pattern,
// but wrapped in a structured interface.
// ============================================================================

// Result of effect execution
struct EffectResult {
    // Accuracy
    bool missed{false};

    // Damage calculation
    uint16_t damage{0};
    uint16_t effectiveness{0x100};  // 0x100 = neutral, 0x200 = 2x, etc.
    bool critical{false};

    // Status
    bool status_applied{false};

    // General failure flag (for guarded sequences)
    bool failed{false};
};

// Damage calculation overrides
struct DamageOverride {
    uint16_t power{0};    // 0 = use move's power
    uint16_t attack{0};   // 0 = use attacker's stat
    uint16_t defense{0};  // 0 = use defender's stat
};

struct BattleContext {
    // ========================================================================
    //                              STATE DOMAINS
    // ========================================================================

    // Domain 1: Field (global)
    logic::state::FieldState* field{nullptr};

    // Domain 2: Sides (per-team)
    logic::state::SideState* attacker_side{nullptr};
    logic::state::SideState* defender_side{nullptr};

    // Domain 3: Slots (per-position)
    logic::state::SlotState* attacker_slot{nullptr};
    logic::state::SlotState* defender_slot{nullptr};

    // Domain 4: Mons (per-pokemon)
    logic::state::MonState* attacker_mon{nullptr};
    logic::state::MonState* defender_mon{nullptr};

    // ========================================================================
    //                             MOVE CONTEXT
    // ========================================================================

    const types::Move* move{nullptr};

    // ========================================================================
    //                           BATTLER IDENTITY
    // ========================================================================

    uint8_t attacker_slot_id{0};
    uint8_t defender_slot_id{0};
    uint8_t attacker_side_id{0};
    uint8_t defender_side_id{0};

    // ========================================================================
    //                           EFFECT EXECUTION
    // ========================================================================

    EffectResult result{};
    DamageOverride override{};

    // Loop iteration counter (for Triple Kick, etc.)
    uint8_t loop_iteration{0};

    // ========================================================================
    //                              HELPERS
    // ========================================================================

    // Check if defender has a substitute up
    [[nodiscard]] constexpr bool defender_has_substitute() const {
        return defender_slot && defender_slot->substitute_hp > 0;
    }

    // Check if attacker is on player's side (side 0)
    [[nodiscard]] constexpr bool attacker_is_player() const {
        return attacker_side_id == 0;
    }

    // Get effective move power (considering overrides)
    [[nodiscard]] constexpr uint16_t effective_power() const {
        return override.power > 0 ? override.power : move->power;
    }
};

}  // namespace dsl
