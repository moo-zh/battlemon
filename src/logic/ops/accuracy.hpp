#pragma once

#include "base.hpp"

namespace logic::ops {

// ============================================================================
//                            CHECK ACCURACY
// ============================================================================
//
// Determines if the move hits or misses.
//
// Domain: Slot (reads accuracy/evasion stages)
// Stage:  Genesis -> AccuracyResolved
// ============================================================================

struct CheckAccuracy : CommandMeta<Domain::Slot, Genesis, AccuracyResolved> {
    static void execute(dsl::BattleContext& ctx) {
        // TODO: Full accuracy formula
        // For now, simple accuracy check

        if (ctx.move->accuracy == 0) {
            // Accuracy 0 means "always hits" (Swift, Aerial Ace, etc.)
            ctx.result.missed = false;
            return;
        }

        // Basic accuracy check (will be expanded with full formula)
        // accuracy * (attacker_acc_stages / defender_eva_stages)
        // For now, just use move accuracy directly

        // Placeholder RNG - in real implementation, use proper RNG
        // For smoke testing, moves always hit
        ctx.result.missed = false;
    }
};

}  // namespace logic::ops
