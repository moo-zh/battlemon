#pragma once

#include "../../dsl/effect.hpp"

namespace logic::routines {

// ============================================================================
//                           STAT EFFECTS
// ============================================================================
//
// Effects that modify stat stages.
// Domain: StatChange (Slot)
// ============================================================================

using namespace dsl;
using namespace dsl::domains;

// ----------------------------------------------------------------------------
// ATTACK_UP - Boost user's Attack by 2 stages (Swords Dance)
// ----------------------------------------------------------------------------

EFFECT(AttackUp2, StatChange) {
    BEGIN(ctx)
    RUN(RaiseUserAtk2)
    END;
}

// ----------------------------------------------------------------------------
// ATTACK_DOWN - Lower defender's Attack by 1 stage (Growl)
// ----------------------------------------------------------------------------

EFFECT(AttackDown1, StatChange) {
    BEGIN(ctx)
    RUN(LowerDefenderAtk1)
    END;
}

// ----------------------------------------------------------------------------
// HAZE - Reset all stat stages on both sides to neutral
// ----------------------------------------------------------------------------
// Resets all stat modifications for all Pokemon in battle.
// Gen III: Also clears some volatile status (not implemented yet).

EFFECT(Haze, StatChange) {
    BEGIN(ctx)
    RUN(ResetAllStats)
    END;
}

}  // namespace logic::routines
