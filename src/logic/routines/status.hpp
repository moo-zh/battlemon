#pragma once

#include "../../dsl/effect.hpp"

namespace logic::routines {

// ============================================================================
//                           STATUS EFFECTS
// ============================================================================
//
// Effects that apply primary status conditions.
// Domain: Status (Mon) or Pure | Scratch for damage+status combos
// ============================================================================

using namespace dsl;
using namespace dsl::domains;

// ----------------------------------------------------------------------------
// POISON_HIT - Damage with 30% poison chance (Poison Sting, Sludge)
// ----------------------------------------------------------------------------

EFFECT(PoisonHit, Pure | Scratch) {
    BEGIN(ctx)
    RUN(CheckAccuracy)
    RUN(CalculateDamage)
    RUN(ApplyDamage)
    RUN_WITH(TryApplyPoison, 30)  // 30% poison chance
    RUN(CheckFaintAfterEffect)
    END;
}

// ----------------------------------------------------------------------------
// POISON - Pure status move (Poison Powder, Toxic)
// ----------------------------------------------------------------------------

EFFECT(Poison, Status) {
    BEGIN(ctx)
    RUN(ApplyPoisonMove)
    END;
}

}  // namespace logic::routines
