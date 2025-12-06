#pragma once

#include "base.hpp"

namespace logic::ops {

// ============================================================================
//                             CHECK FAINT
// ============================================================================
//
// Checks if the defender fainted and marks for replacement.
//
// Domain: Mon (reads HP)
// Stage:  DamageApplied -> FaintChecked
//         EffectApplied -> FaintChecked
// ============================================================================

// From DamageApplied (no secondary effect)
struct CheckFaint : CommandMeta<Domain::Mon, DamageApplied, FaintChecked> {
    static void execute(dsl::BattleContext& ctx) {
        // Check if defender fainted
        if (ctx.defender_mon->is_fainted()) {
            // TODO: Queue switch request, handle Destiny Bond, etc.
            // For now, just note that they fainted
        }
    }
};

// From EffectApplied (after secondary effect)
struct CheckFaintAfterEffect : CommandMeta<Domain::Mon, EffectApplied, FaintChecked> {
    static void execute(dsl::BattleContext& ctx) { CheckFaint::execute(ctx); }
};

}  // namespace logic::ops
