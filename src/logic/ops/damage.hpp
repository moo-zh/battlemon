#pragma once

#include "base.hpp"

namespace logic::ops {

// ============================================================================
//                          CALCULATE DAMAGE
// ============================================================================
//
// Computes damage using the Gen III damage formula.
// Does not apply damage - just calculates and stores in ctx.result.damage.
//
// Domain: Slot | Mon (reads stat stages, base stats)
// Stage:  AccuracyResolved -> DamageCalculated
// ============================================================================

struct CalculateDamage
    : CommandMeta<Domain::Slot | Domain::Mon, AccuracyResolved, DamageCalculated> {
    static void execute(dsl::BattleContext& ctx) {
        // Skip if move missed
        if (ctx.result.missed) {
            ctx.result.damage = 0;
            return;
        }

        // TODO: Full Gen III damage formula
        // ((2 * Level / 5 + 2) * Power * Attack / Defense) / 50 + 2
        // Then apply: STAB, type effectiveness, critical, random factor

        // For smoke testing, use simplified calculation
        uint16_t power = ctx.effective_power();

        // Placeholder damage calculation
        // Real formula will use stats, levels, type chart, etc.
        ctx.result.damage = power;         // Just use power for now
        ctx.result.effectiveness = 0x100;  // Neutral
        ctx.result.critical = false;
    }
};

// ============================================================================
//                            APPLY DAMAGE
// ============================================================================
//
// Subtracts calculated damage from defender's HP (or substitute).
//
// Domain: Slot | Mon (reads substitute, writes HP)
// Stage:  DamageCalculated -> DamageApplied
// ============================================================================

struct ApplyDamage : CommandMeta<Domain::Slot | Domain::Mon, DamageCalculated, DamageApplied> {
    static void execute(dsl::BattleContext& ctx) {
        if (ctx.result.missed || ctx.result.damage == 0) {
            return;
        }

        uint16_t damage = ctx.result.damage;

        // Check for substitute
        if (ctx.defender_has_substitute()) {
            auto& sub_hp = ctx.defender_slot->substitute_hp;
            if (damage >= sub_hp) {
                // Substitute breaks
                damage -= sub_hp;
                sub_hp = 0;
                ctx.defender_slot->clear(logic::state::volatile_flags::SUBSTITUTE);
                // Remaining damage does NOT carry through in Gen III
                return;
            } else {
                sub_hp -= damage;
                return;
            }
        }

        // Apply to actual HP
        ctx.defender_mon->apply_damage(damage);
    }
};

// ============================================================================
//                             DRAIN HP (ABSORB)
// ============================================================================
//
// Heals the attacker by a percentage of damage dealt. Typically 50% for
// Absorb/Mega Drain/Giga Drain. Skips if the move missed, dealt 0 damage,
// or a substitute absorbed all damage.
//
// Domain: Mon (writes attacker's HP)
// Stage:  DamageApplied -> EffectApplied
// ============================================================================

template <uint8_t Percent>
struct DrainHP : CommandMeta<Domain::Mon, DamageApplied, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        if (ctx.result.missed || ctx.result.damage == 0) {
            return;
        }

        // If a substitute is still up, no HP is restored (Gen III behavior)
        if (ctx.defender_has_substitute() &&
            ctx.defender_slot && ctx.defender_slot->substitute_hp > 0) {
            return;
        }

        uint32_t heal = static_cast<uint32_t>(ctx.result.damage) * Percent / 100u;
        if (heal == 0 && ctx.result.damage > 0) {
            heal = 1;  // minimum 1 HP if damage > 0
        }
        ctx.attacker_mon->heal(static_cast<uint16_t>(heal));
    }
};

using DrainHalfHP = DrainHP<50>;

// ============================================================================
//                             RECOIL DAMAGE
// ============================================================================
//
// Damages the attacker by a percentage of damage dealt. Skips if miss/zero.
// Domain: Mon (attacker HP)
// Stage:  DamageApplied -> EffectApplied
// ============================================================================

template <uint8_t Percent>
struct Recoil : CommandMeta<Domain::Mon, DamageApplied, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        if (ctx.result.missed || ctx.result.damage == 0) {
            return;
        }
        uint32_t recoil = static_cast<uint32_t>(ctx.result.damage) * Percent / 100u;
        if (recoil == 0 && ctx.result.damage > 0) {
            recoil = 1;  // minimum 1 if damage occurred
        }
        ctx.attacker_mon->apply_damage(static_cast<uint16_t>(recoil));
    }
};

using RecoilQuarter = Recoil<25>;    // e.g., Take Down
using RecoilThird   = Recoil<33>;    // e.g., Double-Edge (approx)

// ============================================================================
//                           FIXED DAMAGE SETTER
// ============================================================================
//
// Sets ctx.result.damage to a fixed amount (after accuracy).
// Domain: Slot | Mon (damage pipeline context)
// Stage:  AccuracyResolved -> DamageCalculated
// ============================================================================

template <uint16_t Amount>
struct SetFixedDamage : CommandMeta<Domain::Slot | Domain::Mon, AccuracyResolved, DamageCalculated> {
    static void execute(dsl::BattleContext& ctx) {
        if (ctx.result.missed) {
            ctx.result.damage = 0;
            return;
        }
        ctx.result.damage = Amount;
    }
};

// ============================================================================
//                            USER RECOVERY
// ============================================================================
//
// Heals attacker by a percentage of max HP.
// Domain: Mon
// Stage:  Genesis -> EffectApplied
// ============================================================================

template <uint8_t Percent>
struct HealUser : CommandMeta<Domain::Mon, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        if (!ctx.attacker_mon) return;
        uint32_t heal = static_cast<uint32_t>(ctx.attacker_mon->max_hp) * Percent / 100u;
        if (heal == 0 && Percent > 0) heal = 1;
        ctx.attacker_mon->heal(static_cast<uint16_t>(heal));
    }
};

using HealHalf = HealUser<50>;

}  // namespace logic::ops
