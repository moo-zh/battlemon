#pragma once

#include "../../dsl/effect.hpp"

namespace logic::routines {

// ============================================================================
//                           DAMAGE EFFECTS
// ============================================================================
//
// Pure damage effects (Tackle, Pound, Scratch, etc.)
// Domain: Pure (Slot | Mon)
// ============================================================================

using namespace dsl;
using namespace dsl::domains;

// ----------------------------------------------------------------------------
// HIT - Pure damage baseline
// ----------------------------------------------------------------------------
// The simplest damaging move: accuracy check, calculate damage, apply damage,
// check for faint. Used by Tackle, Pound, Scratch, and ~50 other moves.

EFFECT(Hit, Pure) {
    BEGIN(ctx)
    RUN(CheckAccuracy)
    RUN(CalculateDamage)
    RUN(ApplyDamage)
    RUN(CheckFaint)
    END;
}

// ----------------------------------------------------------------------------
// ABSORB - Damaging move that heals the attacker by 50% of damage dealt
// ----------------------------------------------------------------------------

EFFECT(Absorb, Pure) {
    BEGIN(ctx)
    RUN(CheckAccuracy)
    RUN(CalculateDamage)
    RUN(ApplyDamage)
    RUN(DrainHalfHP)
    RUN(CheckFaintAfterEffect)
    END;
}

// ----------------------------------------------------------------------------
// TAKE_DOWN - Damaging move with recoil (25% of damage dealt)
// ----------------------------------------------------------------------------

EFFECT(TakeDown, Pure) {
    BEGIN(ctx)
    RUN(CheckAccuracy)
    RUN(CalculateDamage)
    RUN(ApplyDamage)
    RUN(RecoilQuarter)
    RUN(CheckFaintAfterEffect)
    END;
}

// ----------------------------------------------------------------------------
// DRAGON_RAGE - Fixed 40 damage
// ----------------------------------------------------------------------------

EFFECT(DragonRage, Pure) {
    BEGIN(ctx)
    RUN(CheckAccuracy)
    RUN(SetFixedDamage<40>)
    RUN(ApplyDamage)
    RUN(CheckFaint)
    END;
}

// ----------------------------------------------------------------------------
// RECOVER - Restore user's HP by 50%
// ----------------------------------------------------------------------------

EFFECT(Recover, Domain::Mon) {
    BEGIN(ctx)
    RUN(HealHalf)
    END;
}

}  // namespace logic::routines
