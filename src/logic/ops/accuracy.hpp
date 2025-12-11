#pragma once

#include "../calc/accuracy.hpp"
#include "base.hpp"

namespace logic::ops {

// ============================================================================
//                            CHECK ACCURACY
// ============================================================================
//
// Determines if the move hits or misses using Gen III accuracy formula.
//
// Domain: Slot (reads accuracy/evasion stages)
// Stage:  Genesis -> AccuracyResolved
// ============================================================================

struct CheckAccuracy : CommandMeta<Domain::Slot, Genesis, AccuracyResolved> {
    static void execute(dsl::BattleContext& ctx) {
        // Get stat stages (default to 0 if no slot context)
        int8_t acc_stage = 0;
        int8_t eva_stage = 0;

        if (ctx.attacker_slot) {
            acc_stage = ctx.attacker_slot->accuracy_stage;
        }
        if (ctx.defender_slot) {
            eva_stage = ctx.defender_slot->evasion_stage;
        }

        // Check accuracy using calc module
        bool hits = calc::check_accuracy(ctx.move->accuracy, acc_stage, eva_stage);
        ctx.result.missed = !hits;
    }
};

}  // namespace logic::ops
