#pragma once

#include "../logic/state/context.hpp"
#include "item/dispatch.hpp"
#include "stages.hpp"

namespace dsl {

// ============================================================================
//                         STAGE TRANSITIONS
// ============================================================================
//
// Stage transitions fire item events at stage boundaries.
//
// The key insight: a stage isn't just a type tag - it's a bundle of:
//   1. Invariants that now hold (correctness)
//   2. Events that fire on entry (capability)
//   3. Valid operations from this state (what RUN() accepts)
//
// Stage transitions are where cross-cutting concerns (items, abilities)
// integrate with the move pipeline. The calculation logic stays pure;
// modifications happen at boundaries.
//
// ============================================================================

/// Primary template - default no-op transition
template <typename FromStage, typename ToStage>
struct StageTransition {
    static void execute(BattleContext&) {
        // Default: no events fire
    }

    template <typename Payload>
    static void execute(BattleContext&, Payload&) {
        // Default: no events fire
    }
};

// ============================================================================
//                    TRANSITION SPECIALIZATIONS
// ============================================================================

// ----------------------------------------------------------------------------
// Genesis -> AccuracyResolved
// No item hooks (accuracy modifiers like Bright Powder would go here)
// ----------------------------------------------------------------------------

template <>
struct StageTransition<Genesis, AccuracyResolved> {
    static void execute(BattleContext&) {
        // TODO: Bright Powder, Lax Incense accuracy reduction
        // For now, no-op - accuracy check handles this internally
    }
};

// ----------------------------------------------------------------------------
// AccuracyResolved -> DamageCalculated
// Item hooks: OnPreDamageCalc (Scope Lens, Choice Band, etc.)
// ----------------------------------------------------------------------------

template <>
struct StageTransition<AccuracyResolved, DamageCalculated> {
    template <typename Payload>
    static void execute(BattleContext& ctx, Payload& payload) {
        if (ctx.result.missed) {
            return;
        }

        item::fire_pre_damage_calc(ctx, payload.attack, payload.defense, payload.crit_stage,
                                   payload.power);
    }

    static void execute(BattleContext&) {
        // When invoked without a payload, nothing to do.
    }
};

// ----------------------------------------------------------------------------
// DamageCalculated -> DamageApplied
// Item hooks: OnPreDamageApply (Focus Band)
// ----------------------------------------------------------------------------

template <>
struct StageTransition<DamageCalculated, DamageApplied> {
    static void execute(BattleContext& ctx) {
        // Focus Band check happens inside ApplyDamage for now
        // because it needs to modify the damage before HP subtraction
        (void)ctx;
    }
};

// ----------------------------------------------------------------------------
// DamageApplied -> EffectApplied
// Item hooks: OnPostDamageApply (Shell Bell, King's Rock)
// ----------------------------------------------------------------------------

template <>
struct StageTransition<DamageApplied, EffectApplied> {
    static void execute(BattleContext& ctx) {
        // Post-damage item effects
        if (ctx.result.damage == 0 || ctx.result.missed) {
            return;
        }

        uint16_t attacker_heal = 0;
        uint16_t attacker_recoil = 0;
        bool cause_flinch = false;

        bool target_fainted = ctx.defender_mon && ctx.defender_mon->is_fainted();

        item::fire_post_damage_apply(ctx, ctx.result.damage, ctx.result.critical, target_fainted,
                                     attacker_heal, attacker_recoil, cause_flinch);

        // Apply healing (Shell Bell)
        if (attacker_heal > 0 && ctx.attacker_mon) {
            ctx.attacker_mon->heal(attacker_heal);
        }

        // Apply recoil (Life Orb - not in Gen III base but architecture supports)
        if (attacker_recoil > 0 && ctx.attacker_mon) {
            ctx.attacker_mon->apply_damage(attacker_recoil);
        }

        // Set flinch flag (King's Rock)
        if (cause_flinch && ctx.defender_slot && !target_fainted) {
            ctx.defender_slot->set(logic::state::volatile_flags::FLINCHED);
        }
    }
};

// ----------------------------------------------------------------------------
// Other transitions - no item hooks currently
// ----------------------------------------------------------------------------

template <>
struct StageTransition<EffectApplied, FaintChecked> {
    static void execute(BattleContext&) {}
};

template <>
struct StageTransition<FaintChecked, Terminus> {
    static void execute(BattleContext&) {}
};

// Allow skipping stages (e.g., Genesis -> DamageApplied for status moves)
template <>
struct StageTransition<Genesis, EffectApplied> {
    static void execute(BattleContext&) {}
};

template <>
struct StageTransition<Genesis, FaintChecked> {
    static void execute(BattleContext&) {}
};

template <>
struct StageTransition<Genesis, Terminus> {
    static void execute(BattleContext&) {}
};

template <>
struct StageTransition<DamageApplied, FaintChecked> {
    static void execute(BattleContext&) {}
};

}  // namespace dsl
