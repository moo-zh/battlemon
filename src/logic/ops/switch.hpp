#pragma once

#include "base.hpp"

namespace logic::ops {

// ============================================================================
//                         SWITCH REQUESTS / FLAGS
// ============================================================================
//
// These commands do not perform the switch; they signal the battle engine to
// orchestrate it. The engine is responsible for honoring these flags after
// the effect resolves.
//
// Domain: Slot (attacker side)
// Stage:  Genesis -> Terminus (effect ends after request)
// ============================================================================

struct RequestBatonPass : CommandMeta<Domain::Slot, Genesis, Terminus> {
    static void execute(dsl::BattleContext& ctx) {
        ctx.result.baton_pass = true;
        ctx.result.switch_out = true;
    }
};

// ============================================================================
//                       PURSUIT INTERCEPT REGISTRATION
// ============================================================================
//
// Marks that the current attacker has used Pursuit, enabling the engine to
// check for pre-switch interception (double power) when the defender attempts
// to switch. The actual interception logic must be handled in the turn/switch
// orchestrator; this command only records intent.
//
// Domain: Slot (attacker)
// Stage:  Genesis -> EffectApplied (keeps the effect moving)
// ============================================================================

struct MarkPursuitReady : CommandMeta<Domain::Slot, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        ctx.result.pursuit_intercept = true;
        ctx.result.pursuit_user_slot = ctx.attacker_slot_id;
    }
};

}  // namespace logic::ops
