#pragma once

#include "../calc/damage.hpp"
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

namespace detail {

// Gen III physical/special split is based on type, not move
// Physical: Normal, Fighting, Flying, Poison, Ground, Rock, Bug, Ghost, Steel
// Special: Fire, Water, Grass, Electric, Psychic, Ice, Dragon, Dark
constexpr bool is_physical_type(types::enums::Type type) {
    using enum types::enums::Type;
    switch (type) {
        case NORMAL:
        case FIGHTING:
        case FLYING:
        case POISON:
        case GROUND:
        case ROCK:
        case BUG:
        case GHOST:
        case STEEL:
            return true;
        default:
            return false;
    }
}

}  // namespace detail

struct CalculateDamage
    : CommandMeta<Domain::Slot | Domain::Mon, AccuracyResolved, DamageCalculated> {
    static void execute(dsl::BattleContext& ctx) {
        // Skip if move missed
        if (ctx.result.missed) {
            ctx.result.damage = 0;
            return;
        }

        const auto& attacker = ctx.attacker();
        const auto& defender = ctx.defender();
        const auto& move = *ctx.move;

        // Determine physical vs special based on move type (Gen III)
        bool is_physical = detail::is_physical_type(move.type);

        // Build damage params
        calc::DamageParams params{};
        params.level = attacker.level;
        params.power = ctx.effective_power();
        params.move_type = move.type;

        // Attack/defense based on physical vs special
        if (is_physical) {
            params.attack = ctx.override.attack > 0 ? ctx.override.attack : attacker.attack;
            params.defense = ctx.override.defense > 0 ? ctx.override.defense : defender.defense;
            params.attack_stage =
                ctx.attacker_slot ? ctx.attacker_slot->atk_stage : calc::DEFAULT_STAT_STAGE;
            params.defense_stage =
                ctx.defender_slot ? ctx.defender_slot->def_stage : calc::DEFAULT_STAT_STAGE;
        } else {
            params.attack = ctx.override.attack > 0 ? ctx.override.attack : attacker.sp_attack;
            params.defense = ctx.override.defense > 0 ? ctx.override.defense : defender.sp_defense;
            params.attack_stage =
                ctx.attacker_slot ? ctx.attacker_slot->sp_atk_stage : calc::DEFAULT_STAT_STAGE;
            params.defense_stage =
                ctx.defender_slot ? ctx.defender_slot->sp_def_stage : calc::DEFAULT_STAT_STAGE;
        }

        // Types for STAB and effectiveness
        params.attacker_type1 = attacker.type1;
        params.attacker_type2 = attacker.type2;
        params.defender_type1 = defender.type1;
        params.defender_type2 = defender.type2;

        // Calculate damage
        auto result = calc::calculate_damage(params);

        ctx.result.damage = result.damage;
        ctx.result.effectiveness = result.effectiveness;
        ctx.result.critical = result.critical;
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
        if (ctx.defender_has_substitute() && ctx.defender_slot &&
            ctx.defender_slot->substitute_hp > 0) {
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

using RecoilQuarter = Recoil<25>;  // e.g., Take Down
using RecoilThird = Recoil<33>;    // e.g., Double-Edge (approx)

// ============================================================================
//                           FIXED DAMAGE SETTER
// ============================================================================
//
// Sets ctx.result.damage to a fixed amount (after accuracy).
// Domain: Slot | Mon (damage pipeline context)
// Stage:  AccuracyResolved -> DamageCalculated
// ============================================================================

template <uint16_t Amount>
struct SetFixedDamage
    : CommandMeta<Domain::Slot | Domain::Mon, AccuracyResolved, DamageCalculated> {
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
        if (!ctx.attacker_mon)
            return;
        uint32_t heal = static_cast<uint32_t>(ctx.attacker_mon->max_hp) * Percent / 100u;
        if (heal == 0 && Percent > 0)
            heal = 1;
        ctx.attacker_mon->heal(static_cast<uint16_t>(heal));
    }
};

using HealHalf = HealUser<50>;

}  // namespace logic::ops
