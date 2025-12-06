#pragma once

#include "../../dsl/effect.hpp"

namespace logic::routines {

// ============================================================================
//                        FOUNDATION EFFECTS
// ============================================================================
//
// These 7 effects validate the DSL architecture by covering one effect
// from each domain combination:
//
//   1. HIT           (D)    - Pure damage baseline
//   2. ATTACK_UP     (L)    - Stat boost to user
//   3. ATTACK_DOWN   (L)    - Stat drop to defender
//   4. POISON_HIT    (D+P)  - Damage with status chance
//   5. POISON        (P)    - Pure status
//   6. LIGHT_SCREEN  (S)    - Screen effect
//   7. SANDSTORM     (F)    - Weather effect
//
// Legend: D=Damage, L=Slot(stat stages), P=Mon(status), S=Side, F=Field
// ============================================================================

using namespace dsl;
using namespace dsl::domains;

// ============================================================================
// 1. HIT - Pure damage (Tackle, Pound, etc.)
// ============================================================================

EFFECT(Hit, Pure) {
    BEGIN(ctx)
    RUN(CheckAccuracy)
    RUN(CalculateDamage)
    RUN(ApplyDamage)
    RUN(CheckFaint)
    END;
}

// ============================================================================
// 2. ATTACK_UP - Boost user's Attack (Swords Dance)
// ============================================================================

EFFECT(AttackUp2, StatChange) {
    BEGIN(ctx)
    RUN(RaiseUserAtk2)
    END;
}

// ============================================================================
// 3. ATTACK_DOWN - Lower defender's Attack (Growl)
// ============================================================================

EFFECT(AttackDown1, StatChange) {
    BEGIN(ctx)
    RUN(LowerDefenderAtk1)
    END;
}

// ============================================================================
// 4. POISON_HIT - Damage with poison chance (Poison Sting, Sludge)
// ============================================================================

EFFECT(PoisonHit, Pure) {
    BEGIN(ctx)
    RUN(CheckAccuracy)
    RUN(CalculateDamage)
    RUN(ApplyDamage)
    RUN_WITH(TryApplyPoison, 30)  // 30% poison chance
    RUN(CheckFaintAfterEffect)
    END;
}

// ============================================================================
// 5. POISON - Pure status (Poison Powder, Toxic)
// ============================================================================

EFFECT(Poison, Status) {
    BEGIN(ctx)
    RUN(ApplyPoisonMove)
    END;
}

// ============================================================================
// 6. LIGHT_SCREEN - Screen effect
// ============================================================================

EFFECT(LightScreen, Screen) {
    BEGIN(ctx)
    RUN(SetLightScreen)
    END;
}

// ============================================================================
// 7. SANDSTORM - Weather effect
// ============================================================================

EFFECT(Sandstorm, Domain::Field) {
    BEGIN(ctx)
    RUN(SetSandstorm)
    END;
}

}  // namespace logic::routines
