#pragma once

#include <cstdint>

// Forward declare to avoid circular dependency
namespace dsl {
struct BattleContext;
}

namespace dsl::item {

// ============================================================================
//                            ITEM EVENT TYPES
// ============================================================================
//
// Events fire at stage boundaries, not during calculations.
// Each event has:
//   - A firing point (stage transition)
//   - Event data (mutable refs to params or results)
//   - Participant specification (attacker, defender, or both)
//
// Events use mutable references for parameters they can modify and const
// references for read-only context. This avoids type-state mutations while
// still allowing item effects to influence calculations.
//
// Reference: ADR-0004 (Item Effect Architecture)
// ============================================================================

// ----------------------------------------------------------------------------
// Move Pipeline Events
// ----------------------------------------------------------------------------

/// Fires: AccuracyResolved -> DamageCalculated transition
/// Modifies: Damage calculation inputs before pure calc runs
/// Use: Choice Band, Light Ball, Scope Lens, type-boost items
struct OnPreDamageCalc {
    uint16_t& attack;     // Choice Band (1.5x), Light Ball (2x)
    uint16_t& defense;    // Metal Powder (Ditto, 2x)
    uint8_t& crit_stage;  // Scope Lens (+1), Lucky Punch (Chansey, +2)
    uint16_t& power;      // Type-boost items (1.1x)

    const BattleContext& ctx;
};

/// Fires: DamageCalculated -> DamageApplied transition (pre-apply)
/// Modifies: Calculated damage before HP is changed
/// Use: Focus Band (survive fatal), defensive items
struct OnPreDamageApply {
    uint16_t& damage;            // Can reduce
    const uint16_t defender_hp;  // For fatal check
    bool& survived_fatal;        // Set true if item saved defender

    const BattleContext& ctx;
};

/// Fires: DamageCalculated -> DamageApplied transition (post-apply)
/// Reacts: To damage dealt, can trigger side effects
/// Use: Shell Bell (heal), King's Rock (flinch), Life Orb (recoil)
struct OnPostDamageApply {
    const uint16_t damage_dealt;
    const bool was_critical;
    const bool target_fainted;

    uint16_t& attacker_heal;    // Shell Bell (1/8 damage)
    uint16_t& attacker_recoil;  // Life Orb (1/10 max HP)
    bool& cause_flinch;         // King's Rock (10%)

    const BattleContext& ctx;
};

// ----------------------------------------------------------------------------
// Turn Pipeline Events
// ----------------------------------------------------------------------------

/// Fires: TurnGenesis -> PriorityDetermined transition
/// Modifies: Turn order determination
/// Use: Quick Claw (20% priority boost)
struct OnTurnStart {
    bool& priority_boost;  // Quick Claw sets true

    const BattleContext& ctx;
};

/// Fires: ActionsResolved -> TurnEnd transition
/// Modifies: End-of-turn HP changes
/// Use: Leftovers (1/16 heal), Black Sludge
struct OnTurnEnd {
    uint16_t& heal_amount;    // Leftovers
    uint16_t& damage_amount;  // Black Sludge (wrong type)

    const BattleContext& ctx;
};

}  // namespace dsl::item
