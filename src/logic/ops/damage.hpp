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

}  // namespace logic::ops
