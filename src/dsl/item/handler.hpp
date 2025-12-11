#pragma once

#include "../../types/enums/item.hpp"
#include "../../util/random.hpp"
#include "events.hpp"

namespace dsl::item {

// ============================================================================
//                         ITEM HANDLER TEMPLATE
// ============================================================================
//
// Static dispatch via template specialization. No vtables, no runtime cost.
//
// Pattern:
//   1. Primary template does nothing (default behavior)
//   2. Specializations implement item-specific behavior
//   3. Dispatcher calls handler for held item via switch
//
// The `handles` constant allows if constexpr elimination of non-handling
// branches at compile time.
//
// ============================================================================

/// Primary template - default no-op handler
template <types::enums::Item ItemId, typename Event>
struct ItemHandler {
    /// Returns true if this item responds to this event
    static constexpr bool handles = false;

    /// No-op execute (never called if handles=false)
    static void execute(Event&) {}
};

// ============================================================================
//                      HANDLER SPECIALIZATIONS
// ============================================================================

// ----------------------------------------------------------------------------
// SCOPE LENS - +1 crit stage
// Gen III: Raises critical hit ratio by one stage
// ----------------------------------------------------------------------------

template <>
struct ItemHandler<types::enums::Item::SCOPE_LENS, OnPreDamageCalc> {
    static constexpr bool handles = true;

    static void execute(OnPreDamageCalc& event) {
        // Crit stages cap at 4 in Gen III (guaranteed crit)
        if (event.crit_stage < 4) {
            event.crit_stage += 1;
        }
    }
};

// ----------------------------------------------------------------------------
// CHOICE BAND - 1.5x Attack (physical moves only)
// Gen III: Boosts Attack by 50%, locks into first move used
// Note: Move locking is handled by engine, not here
// ----------------------------------------------------------------------------

template <>
struct ItemHandler<types::enums::Item::CHOICE_BAND, OnPreDamageCalc> {
    static constexpr bool handles = true;

    static void execute(OnPreDamageCalc& event) {
        // Choice Band only boosts Attack, which means physical moves only.
        // The caller (stage transition) should only fire this for physical moves,
        // but we apply unconditionally here - the event.attack IS the attack stat.
        event.attack = static_cast<uint16_t>(static_cast<uint32_t>(event.attack) * 3 / 2);
    }
};

// ----------------------------------------------------------------------------
// FOCUS BAND - 12% chance to survive fatal hit at 1 HP
// Gen III: ~12% (approximately 1/8) chance to endure
// ----------------------------------------------------------------------------

template <>
struct ItemHandler<types::enums::Item::FOCUS_BAND, OnPreDamageApply> {
    static constexpr bool handles = true;

    static void execute(OnPreDamageApply& event) {
        // Only triggers if this would be fatal
        if (event.damage >= event.defender_hp) {
            // 12% = 12/100 chance
            if (util::random::Random(100) < 12) {
                event.damage = event.defender_hp - 1;  // Leave at 1 HP
                event.survived_fatal = true;
            }
        }
    }
};

// ----------------------------------------------------------------------------
// KING'S ROCK - 10% flinch on damaging hit
// Gen III: 10% chance to flinch, stacks with move's flinch chance additively
// ----------------------------------------------------------------------------

template <>
struct ItemHandler<types::enums::Item::KINGS_ROCK, OnPostDamageApply> {
    static constexpr bool handles = true;

    static void execute(OnPostDamageApply& event) {
        // Only if we dealt damage and target didn't faint
        if (event.damage_dealt > 0 && !event.target_fainted) {
            // 10% = 1/10 chance
            if (util::random::Random(10) == 0) {
                event.cause_flinch = true;
            }
        }
    }
};

// ----------------------------------------------------------------------------
// SHELL BELL - Heal 1/8 of damage dealt
// Gen III: Restores HP equal to 1/8 of damage inflicted
// ----------------------------------------------------------------------------

template <>
struct ItemHandler<types::enums::Item::SHELL_BELL, OnPostDamageApply> {
    static constexpr bool handles = true;

    static void execute(OnPostDamageApply& event) {
        if (event.damage_dealt > 0) {
            uint16_t heal = event.damage_dealt / 8;
            if (heal == 0)
                heal = 1;  // Minimum 1 HP
            event.attacker_heal = heal;
        }
    }
};

// ----------------------------------------------------------------------------
// LEFTOVERS - Heal 1/16 max HP at turn end
// Gen III: Restores 1/16 of max HP each turn
// Note: Requires max_hp from context - will be passed via event
// ----------------------------------------------------------------------------

template <>
struct ItemHandler<types::enums::Item::LEFTOVERS, OnTurnEnd> {
    static constexpr bool handles = true;

    static void execute(OnTurnEnd& event);  // Defined in .cpp - needs context access
};

// ----------------------------------------------------------------------------
// QUICK CLAW - 20% chance to move first
// Gen III: 20% (approximately 1/5) chance for priority boost
// ----------------------------------------------------------------------------

template <>
struct ItemHandler<types::enums::Item::QUICK_CLAW, OnTurnStart> {
    static constexpr bool handles = true;

    static void execute(OnTurnStart& event) {
        // 20% = 1/5 chance
        if (util::random::Random(5) == 0) {
            event.priority_boost = true;
        }
    }
};

}  // namespace dsl::item
