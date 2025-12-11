#pragma once

#include "../../logic/state/context.hpp"
#include "handler.hpp"

namespace dsl::item {

// ============================================================================
//                         ITEM EVENT DISPATCHER
// ============================================================================
//
// Routes events to item handlers via switch-based dispatch.
// Uses `if constexpr` to eliminate non-handling branches at compile time.
//
// The switch covers all items that have ANY handler specialization.
// Items without handlers for a given event compile to no-ops.
//
// ============================================================================

/// Dispatch an event to the appropriate item handler
template <typename Event>
void dispatch(types::enums::Item item, Event& event) {
    using enum types::enums::Item;

    // clang-format off
    switch (item) {
        // ==== Utility items with battle effects ====
        case SCOPE_LENS:
            if constexpr (ItemHandler<SCOPE_LENS, Event>::handles)
                ItemHandler<SCOPE_LENS, Event>::execute(event);
            break;

        case CHOICE_BAND:
            if constexpr (ItemHandler<CHOICE_BAND, Event>::handles)
                ItemHandler<CHOICE_BAND, Event>::execute(event);
            break;

        case FOCUS_BAND:
            if constexpr (ItemHandler<FOCUS_BAND, Event>::handles)
                ItemHandler<FOCUS_BAND, Event>::execute(event);
            break;

        case KINGS_ROCK:
            if constexpr (ItemHandler<KINGS_ROCK, Event>::handles)
                ItemHandler<KINGS_ROCK, Event>::execute(event);
            break;

        case SHELL_BELL:
            if constexpr (ItemHandler<SHELL_BELL, Event>::handles)
                ItemHandler<SHELL_BELL, Event>::execute(event);
            break;

        case LEFTOVERS:
            if constexpr (ItemHandler<LEFTOVERS, Event>::handles)
                ItemHandler<LEFTOVERS, Event>::execute(event);
            break;

        case QUICK_CLAW:
            if constexpr (ItemHandler<QUICK_CLAW, Event>::handles)
                ItemHandler<QUICK_CLAW, Event>::execute(event);
            break;

        // ==== Signature items (species-specific) ====
        // TODO: Light Ball (Pikachu), Metal Powder (Ditto), etc.
        // These need species checks in the stage transition before dispatch

        // ==== Type boost items ====
        // TODO: These modify power for matching types

        // ==== Berries ====
        // TODO: Consumable effects

        // ==== Non-battle items or no handlers ====
        default:
            break;
    }
    // clang-format on
}

// ============================================================================
//                      CONVENIENCE FIRE FUNCTIONS
// ============================================================================
//
// These functions construct events and dispatch them to the appropriate items.
// Called from stage transitions to fire item hooks.
//
// ============================================================================

/// Fire OnPreDamageCalc for attacker's item
inline void fire_pre_damage_calc(BattleContext& ctx, uint16_t& attack, uint16_t& defense,
                                 uint8_t& crit_stage, uint16_t& power) {
    if (!ctx.attacker_slot)
        return;

    types::enums::Item item = ctx.attacker_slot->held_item;
    if (item == types::enums::Item::NONE)
        return;
    if (ctx.attacker_slot->item_consumed)
        return;

    OnPreDamageCalc event{attack, defense, crit_stage, power, ctx};
    dispatch(item, event);
}

/// Fire OnPreDamageApply for defender's item
inline void fire_pre_damage_apply(BattleContext& ctx, uint16_t& damage, uint16_t defender_hp,
                                  bool& survived_fatal) {
    if (!ctx.defender_slot)
        return;

    types::enums::Item item = ctx.defender_slot->held_item;
    if (item == types::enums::Item::NONE)
        return;
    if (ctx.defender_slot->item_consumed)
        return;

    OnPreDamageApply event{damage, defender_hp, survived_fatal, ctx};
    dispatch(item, event);
}

/// Fire OnPostDamageApply for attacker's and defender's items
inline void fire_post_damage_apply(BattleContext& ctx, uint16_t damage_dealt, bool was_critical,
                                   bool target_fainted, uint16_t& attacker_heal,
                                   uint16_t& attacker_recoil, bool& cause_flinch) {
    OnPostDamageApply event{
        damage_dealt, was_critical, target_fainted, attacker_heal, attacker_recoil,
        cause_flinch, ctx};

    // Attacker's item (Shell Bell, King's Rock)
    if (ctx.attacker_slot && !ctx.attacker_slot->item_consumed) {
        types::enums::Item item = ctx.attacker_slot->held_item;
        if (item != types::enums::Item::NONE) {
            dispatch(item, event);
        }
    }

    // Defender's item could have post-hit triggers too
    // (none in Gen III base, but architecture supports it)
}

/// Fire OnTurnStart for a slot's item
inline void fire_turn_start(BattleContext& ctx, bool& priority_boost) {
    if (!ctx.attacker_slot)
        return;

    types::enums::Item item = ctx.attacker_slot->held_item;
    if (item == types::enums::Item::NONE)
        return;
    if (ctx.attacker_slot->item_consumed)
        return;

    OnTurnStart event{priority_boost, ctx};
    dispatch(item, event);
}

/// Fire OnTurnEnd for a slot's item
inline void fire_turn_end(BattleContext& ctx, uint16_t& heal_amount, uint16_t& damage_amount) {
    if (!ctx.attacker_slot)
        return;

    types::enums::Item item = ctx.attacker_slot->held_item;
    if (item == types::enums::Item::NONE)
        return;
    if (ctx.attacker_slot->item_consumed)
        return;

    OnTurnEnd event{heal_amount, damage_amount, ctx};
    dispatch(item, event);
}

}  // namespace dsl::item
