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

}  // namespace logic::ops
