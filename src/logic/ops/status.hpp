#pragma once

#include "base.hpp"

namespace logic::ops {

// ============================================================================
//                          STATUS APPLICATION
// ============================================================================
//
// ops for applying primary status conditions (burn, freeze, etc.)
// These check for immunity and existing status before applying.
//
// Domain: Mon (writes status)
// Stage:  DamageApplied -> EffectApplied (for secondary effects)
//         Genesis -> EffectApplied (for pure status moves)
// ============================================================================

// Try to apply a status with a percentage chance
template <logic::state::Status S>
struct TryApplyStatus : CommandMeta<Domain::Mon, DamageApplied, EffectApplied> {
    static void execute(dsl::BattleContext& ctx, uint8_t chance) {
        // Skip if missed
        if (ctx.result.missed) {
            return;
        }

        // Can't status if already statused
        if (ctx.defender_mon->has_status()) {
            return;
        }

        // TODO: Type immunities
        // - Fire immune to burn
        // - Ice immune to freeze
        // - Poison/Steel immune to poison
        // - Electric immune to paralysis (in some gens)

        // TODO: Ability immunities (Limber, Water Veil, etc.)

        // TODO: Safeguard check

        // Roll for chance
        // For smoke testing, always apply if chance > 0
        if (chance > 0) {
            ctx.defender_mon->status = S;
            ctx.result.status_applied = true;

            // Set sleep turns for sleep
            if constexpr (S == logic::state::Status::SLEEP) {
                ctx.defender_mon->sleep_turns = 3;  // TODO: Random 1-3 in Gen III
            }
        }
    }
};

// Convenient aliases
using TryApplyBurn = TryApplyStatus<logic::state::Status::BURN>;
using TryApplyFreeze = TryApplyStatus<logic::state::Status::FREEZE>;
using TryApplyParalyze = TryApplyStatus<logic::state::Status::PARALYSIS>;
using TryApplyPoison = TryApplyStatus<logic::state::Status::POISON>;
using TryApplyToxic = TryApplyStatus<logic::state::Status::TOXIC>;
using TryApplySleep = TryApplyStatus<logic::state::Status::SLEEP>;

// ============================================================================
//                      PURE STATUS MOVES
// ============================================================================
//
// For moves that ONLY apply status (Thunder Wave, Will-O-Wisp, etc.)
// These run from Genesis since there's no damage phase.
// ============================================================================

template <logic::state::Status S>
struct ApplyStatusMove : CommandMeta<Domain::Mon, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        // Can't status if already statused
        if (ctx.defender_mon->has_status()) {
            ctx.result.failed = true;
            return;
        }

        // TODO: Type and ability immunities

        ctx.defender_mon->status = S;
        ctx.result.status_applied = true;

        if constexpr (S == logic::state::Status::SLEEP) {
            ctx.defender_mon->sleep_turns = 3;
        }
    }
};

using ApplyParalyzeMove = ApplyStatusMove<logic::state::Status::PARALYSIS>;
using ApplyPoisonMove = ApplyStatusMove<logic::state::Status::POISON>;
using ApplyToxicMove = ApplyStatusMove<logic::state::Status::TOXIC>;
using ApplySleepMove = ApplyStatusMove<logic::state::Status::SLEEP>;
using ApplyBurnMove = ApplyStatusMove<logic::state::Status::BURN>;

// ============================================================================
//                    FIXED-CHANCE STATUS ops
// ============================================================================
//
// For declarative effect definitions, we need ops with the chance
// baked into the type. These are used with Seq<...> style effects.
// ============================================================================

template <logic::state::Status S, uint8_t Chance>
struct TryApplyStatusChance : CommandMeta<Domain::Mon, DamageApplied, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) { TryApplyStatus<S>::execute(ctx, Chance); }
};

// ============================================================================
//                         VOLATILE STATUS (Flinch)
// ============================================================================
//
// Flinch is a per-turn volatile that prevents the target from moving.
// Applied as a secondary effect after damage.
//
// Domain: Slot (writes volatile flags)
// Stage:  DamageApplied -> EffectApplied
// ============================================================================

struct TryApplyFlinch : CommandMeta<Domain::Slot, DamageApplied, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        // Skip if missed
        if (ctx.result.missed) {
            return;
        }

        // Can only flinch if target hasn't moved yet this turn
        if (ctx.defender_slot->moved_this_turn) {
            return;
        }

        // TODO: Roll for chance (Sky Attack = 30%)
        // For smoke testing, always apply
        ctx.defender_slot->set(logic::state::volatile_flags::FLINCHED);
    }
};

// ============================================================================
//                         CHARGING MOVES
// ============================================================================
//
// Two-turn moves like Sky Attack, Solar Beam, Fly, etc.
// BeginCharge: Sets charging state, skips attack this turn
// ClearCharge: Clears charging state after attack executes
//
// Domain: Slot (writes charging_move, volatile flags)
// Stage:  Genesis -> FaintChecked (BeginCharge ends the effect early)
//         Genesis -> AccuracyResolved (ClearCharge runs before accuracy)
// ============================================================================

struct BeginCharge : CommandMeta<Domain::Slot, Genesis, FaintChecked> {
    static void execute(dsl::BattleContext& ctx) {
        // Store the move being charged
        // In a real impl, this would be the move ID from ctx.move
        ctx.attacker_slot->charging_move = 1;  // Placeholder non-zero value
        ctx.attacker_slot->set(logic::state::volatile_flags::CHARGING);

        // For semi-invulnerable moves (Fly, Dig, Dive), also set SEMI_INVULN
        // Sky Attack doesn't grant semi-invulnerability in Gen III
    }
};

struct ClearCharge : CommandMeta<Domain::Slot, Genesis, AccuracyResolved> {
    static void execute(dsl::BattleContext& ctx) {
        ctx.attacker_slot->charging_move = 0;
        ctx.attacker_slot->clear(logic::state::volatile_flags::CHARGING);
    }
};

// ============================================================================
//                           MAGIC COAT (BOUNCE)
// ============================================================================
//
// Sets the per-turn "bounce" flag to reflect eligible status moves.
// Actual interception is handled by the battle engine when selecting/using
// a target: if defender->bounce_move and move.flags.magic_coat_affected(),
// redirect the move back to the user and clear bounce_move.
//
// Domain: Slot (writes per-turn protection flag)
// Stage:  Genesis -> EffectApplied
// ============================================================================

struct SetMagicCoat : CommandMeta<Domain::Slot, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        if (ctx.attacker_slot) {
            ctx.attacker_slot->bounce_move = true;
        }
    }
};

// ============================================================================
//                         PERISH SONG
// ============================================================================
//
// Applies Perish Song to ALL battlers in the field (including the user).
// Each affected Pokemon faints after 3 turns unless they switch out.
//
// Gen III behavior:
// - Affects all Pokemon on the field
// - Pokemon already affected are not re-affected (counter doesn't reset)
// - Soundproof blocks the effect
//
// Domain: Slot (writes volatile flags to all slots)
// Stage:  Genesis -> EffectApplied
// ============================================================================

struct ApplyPerishSong : CommandMeta<Domain::Slot, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        bool any_affected = false;

        for (uint8_t i = 0; i < ctx.active_slot_count; ++i) {
            auto* slot = ctx.slots[i];
            auto* mon = ctx.mons[i];

            if (!slot || !mon)
                continue;

            // Skip fainted Pokemon
            if (mon->is_fainted())
                continue;

            // Skip if already affected by Perish Song
            if (slot->has(logic::state::volatile_flags::PERISH_SONG))
                continue;

            // TODO: Check for Soundproof ability

            // Apply Perish Song
            slot->set(logic::state::volatile_flags::PERISH_SONG);
            slot->perish_count = 3;
            any_affected = true;
        }

        // Move fails if no one was affected (all already had it)
        if (!any_affected) {
            ctx.result.failed = true;
        }
    }
};

}  // namespace logic::ops
