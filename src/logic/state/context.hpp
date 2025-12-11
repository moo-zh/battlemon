#pragma once

#include <cassert>
#include <cstdint>

#include "field.hpp"
#include "mon.hpp"
#include "side.hpp"
#include "slot.hpp"

#include "../../types/enums/type.hpp"
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

    // Switch-related requests (handled by battle engine)
    bool switch_out{false};
    bool baton_pass{false};
    bool pursuit_intercept{false};
    uint8_t pursuit_user_slot{0xFF};
};

// Computed active mon info (set by BattleEngine before effect execution)
// Contains pre-calculated stats that have already applied nature, IVs, EVs.
// Effects use these directly; stat stages are applied during damage calc.
struct ActiveMon {
    uint8_t level{50};  // Battle Factory uses level 50

    // Computed stats (before stat stage modifiers)
    uint16_t attack{100};
    uint16_t defense{100};
    uint16_t sp_attack{100};
    uint16_t sp_defense{100};
    uint16_t speed{100};

    // Types for STAB and effectiveness calculations
    types::enums::Type type1{types::enums::Type::NONE};
    types::enums::Type type2{types::enums::Type::NONE};
};

// Damage calculation overrides (for moves that ignore normal stats)
struct DamageOverride {
    uint16_t power{0};    // 0 = use move's power
    uint16_t attack{0};   // 0 = use active mon's attack stat
    uint16_t defense{0};  // 0 = use active mon's defense stat
};

// Maximum slots in battle (2 for singles, 4 for doubles)
inline constexpr uint8_t MAX_BATTLE_SLOTS = 2;

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
    // Individual slot pointers (for current attacker/defender)
    logic::state::SlotState* attacker_slot{nullptr};
    logic::state::SlotState* defender_slot{nullptr};

    // All slots array (for iteration - PerishSong, Haze in doubles, etc.)
    logic::state::SlotState* slots[MAX_BATTLE_SLOTS]{nullptr, nullptr};
    logic::state::MonState* mons[MAX_BATTLE_SLOTS]{nullptr, nullptr};
    uint8_t active_slot_count{MAX_BATTLE_SLOTS};

    // Domain 4: Mons (per-pokemon)
    logic::state::MonState* attacker_mon{nullptr};
    logic::state::MonState* defender_mon{nullptr};

    // ========================================================================
    //                         ACTIVE MON INFO
    // ========================================================================

    // Computed stats for damage calculation (set by BattleEngine)
    ActiveMon* attacker_active{nullptr};
    ActiveMon* defender_active{nullptr};

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

    // Get attacker's active mon info (asserts non-null)
    [[nodiscard]] const ActiveMon& attacker() const {
        assert(attacker_active && "attacker_active must be set for damage calc");
        return *attacker_active;
    }

    // Get defender's active mon info (asserts non-null)
    [[nodiscard]] const ActiveMon& defender() const {
        assert(defender_active && "defender_active must be set for damage calc");
        return *defender_active;
    }
};

}  // namespace dsl
