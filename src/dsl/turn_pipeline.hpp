#pragma once

/**
 * @file turn_pipeline.hpp
 * @brief Turn-level event firing for item/ability hooks.
 *
 * Unlike the move pipeline which uses type-state tracking, the turn pipeline
 * is lighter weight - it provides fire_* functions that the engine calls
 * at appropriate points.
 *
 * The engine maintains turn state imperatively; the pipeline just provides
 * structured hook points.
 */

#include "../logic/state/context.hpp"
#include "item/dispatch.hpp"
#include "turn_stages.hpp"

namespace dsl::turn {

// ============================================================================
//                       TURN EVENT FIRING
// ============================================================================
//
// These functions fire item/ability events at turn-level stage transitions.
// The engine calls them at the appropriate points in turn execution.
//
// ============================================================================

/**
 * @brief Fire events for TurnGenesis -> PriorityDetermined transition.
 *
 * Called before speed/priority determination.
 * Item hooks: OnTurnStart (Quick Claw)
 *
 * @param ctx Battle context with attacker_slot set to the battler to check
 * @param[out] priority_boost Set true if item grants priority boost
 */
inline void fire_priority_events(BattleContext& ctx, bool& priority_boost) {
    item::fire_turn_start(ctx, priority_boost);
}

/**
 * @brief Fire events for ActionsResolved -> TurnEnd transition.
 *
 * Called after all moves complete, before end-of-turn effects.
 * Item hooks: OnTurnEnd (Leftovers, Black Sludge)
 *
 * @param ctx Battle context with attacker_slot set to the battler to check
 * @param[out] heal_amount HP to heal (Leftovers)
 * @param[out] damage_amount HP to damage (Black Sludge on wrong type)
 */
inline void fire_end_of_turn_events(BattleContext& ctx, uint16_t& heal_amount,
                                    uint16_t& damage_amount) {
    item::fire_turn_end(ctx, heal_amount, damage_amount);
}

// ============================================================================
//                   PER-SLOT EVENT FIRING HELPER
// ============================================================================

/**
 * @brief Fire turn-start events for a specific slot.
 *
 * Convenience wrapper that sets up context and fires events.
 *
 * @param ctx Battle context
 * @param slot_state Slot to check item for
 * @param[out] priority_boost Set true if item grants priority
 */
inline void fire_turn_start_for_slot(BattleContext& ctx, logic::state::SlotState* slot_state,
                                     bool& priority_boost) {
    if (!slot_state)
        return;

    // Temporarily set attacker_slot for the check
    auto* prev_slot = ctx.attacker_slot;
    ctx.attacker_slot = slot_state;

    fire_priority_events(ctx, priority_boost);

    ctx.attacker_slot = prev_slot;
}

/**
 * @brief Fire turn-end events for a specific slot.
 *
 * Convenience wrapper that sets up context and fires events.
 *
 * @param ctx Battle context
 * @param slot_state Slot to check item for
 * @param mon_state Mon state for HP operations
 */
inline void fire_turn_end_for_slot(BattleContext& ctx, logic::state::SlotState* slot_state,
                                   logic::state::MonState* mon_state) {
    if (!slot_state || !mon_state)
        return;
    if (mon_state->is_fainted())
        return;

    // Temporarily set context for the check
    auto* prev_slot = ctx.attacker_slot;
    auto* prev_mon = ctx.attacker_mon;
    ctx.attacker_slot = slot_state;
    ctx.attacker_mon = mon_state;

    uint16_t heal = 0;
    uint16_t damage = 0;

    fire_end_of_turn_events(ctx, heal, damage);

    // Apply effects
    if (heal > 0) {
        // Calculate heal based on max_hp for Leftovers (1/16)
        uint16_t actual_heal = mon_state->max_hp / 16;
        if (actual_heal == 0)
            actual_heal = 1;
        mon_state->heal(actual_heal);
    }

    if (damage > 0) {
        mon_state->apply_damage(damage);
    }

    ctx.attacker_slot = prev_slot;
    ctx.attacker_mon = prev_mon;
}

}  // namespace dsl::turn
