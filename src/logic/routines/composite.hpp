#pragma once

#include "../../dsl/effect.hpp"

namespace logic::routines {

// ============================================================================
//                         COMPOSITE EFFECTS
// ============================================================================
//
// Complex effects that combine multiple mechanics or require special control
// flow (multi-turn, branching, etc.)
// ============================================================================

using namespace dsl;
using namespace dsl::domains;
using namespace logic::ops;

// ----------------------------------------------------------------------------
// SKY_ATTACK - Two-turn charging move
// ----------------------------------------------------------------------------
// Turn 1: User charges (becomes semi-invulnerable in some games, not Gen III)
// Turn 2: User attacks with high power and 30% flinch chance
//
// Control flow:
//   if (not charging) -> begin charge, set charging_move
//   else              -> execute attack, clear charging_move

namespace detail {
// Attack sequence for turn 2
using SkyAttackHit = Seq<ClearCharge, CheckAccuracy, CalculateDamage, ApplyDamage, TryApplyFlinch,
                         CheckFaintAfterEffect>;

// Whole control flow as an action for macro-friendly usage
using SkyAttackAction =
    Match<FaintChecked, Branch<when::NotCharging, Do<BeginCharge>>, Otherwise<SkyAttackHit>>;
}  // namespace detail

EFFECT(SkyAttack, Pure) {
    BEGIN(ctx)
    RUN_ACTION(detail::SkyAttackAction)
    END;
}

// ----------------------------------------------------------------------------
// MAGIC COAT - Reflect eligible status moves for the remainder of the turn
// ----------------------------------------------------------------------------
// Engine hook: when targeting a defender, if defender_slot->bounce_move is true
// and move.flags.magic_coat_affected(), redirect the move back to the user and
// clear bounce_move. This command just sets the per-turn flag.

EFFECT(MagicCoat, StatChange) {
    BEGIN(ctx)
    RUN(SetMagicCoat)
    END;
}

// ----------------------------------------------------------------------------
// BATON PASS - Request switch while passing volatile state
// ----------------------------------------------------------------------------
// This command signals the engine to perform a switch-out with Baton Pass
// semantics. Engine must call clear_for_baton_pass() on the outgoing slot
// and handle incoming selection.

EFFECT(BatonPass, StatChange) {
    BEGIN(ctx)
    RUN(RequestBatonPass)
    END;
}

// ----------------------------------------------------------------------------
// PURSUIT - Damaging move with switch interception hook
// ----------------------------------------------------------------------------
// Normal hit sequence; also registers pursuit intent for pre-switch intercept.
// Engine must check ctx.result.pursuit_intercept when defender switches and
// resolve a pre-switch hit with doubled power.

EFFECT(Pursuit, Pure) {
    BEGIN(ctx)
    RUN(MarkPursuitReady)
    RUN(CheckAccuracy)
    RUN(CalculateDamage)
    RUN(ApplyDamage)
    RUN(CheckFaint)
    END;
}

// ----------------------------------------------------------------------------
// PERISH_SONG - Affects all battlers with 3-turn KO countdown
// ----------------------------------------------------------------------------
// All Pokemon on the field (including the user) receive Perish Song status.
// At the end of each turn, perish_count decrements. When it reaches 0,
// the Pokemon faints.
//
// This effect validates the all-battler iteration pattern using the
// slots[] array in BattleContext.

EFFECT(PerishSong, StatChange) {
    BEGIN(ctx)
    RUN(ApplyPerishSong)
    END;
}

}  // namespace logic::routines
