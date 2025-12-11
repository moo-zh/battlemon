#pragma once

#include "types/calc.hpp"
#include "types/enums/type.hpp"
#include "util/assert.hpp"

namespace logic::calc {

using types::calc::Effectiveness;

// ============================================================================
//                         TYPE EFFECTIVENESS
// ============================================================================
//
// Gen III type effectiveness chart.
//
// Multiplier encoding (fixed-point, divide by 10 to get actual multiplier):
//   0  = immune (0x)
//   5  = not very effective (0.5x)
//   10 = neutral (1x)
//   20 = super effective (2x)
//
// NONE type is treated as neutral against everything, enabling uniform
// dual-type calculation: mono-types simply pass NONE as their second type.
//
// Reference: pokeemerald/src/battle_main.c gTypeEffectiveness[]
// ============================================================================

namespace effectiveness {
constexpr uint8_t IMMUNE = 0;
constexpr uint8_t NOT_VERY = 5;
constexpr uint8_t NEUTRAL = 10;
constexpr uint8_t SUPER = 20;

// Dual-type effectiveness thresholds (NEUTRAL * NEUTRAL = 100)
constexpr uint16_t DUAL_NEUTRAL = NEUTRAL * NEUTRAL;
}  // namespace effectiveness

// Type indices matching types::enums::Type
// (NONE = 0, NORMAL = 1, ..., DARK = 17)
constexpr uint8_t TYPE_COUNT = 18;

// Type effectiveness table [attacking_type][defending_type]
// Returns multiplier * 10 (so 10 = neutral, 20 = super effective, etc.)
// Row/column 0 (NONE) is neutral against everything.
//
// clang-format off
constexpr uint8_t TYPE_CHART[TYPE_COUNT][TYPE_COUNT] = {
    //           NONE NRM  FGT  FLY  PSN  GRD  RCK  BUG  GHO  STL  FIR  WTR  GRS  ELC  PSY  ICE  DRG  DRK
    /* NONE */  { 10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10 },
    /* NRM  */  { 10,  10,  10,  10,  10,  10,   5,  10,   0,   5,  10,  10,  10,  10,  10,  10,  10,  10 },
    /* FGT  */  { 10,  20,  10,   5,   5,  10,  20,   5,   0,  20,  10,  10,  10,  10,   5,  20,  10,  20 },
    /* FLY  */  { 10,  10,  20,  10,  10,  10,   5,  20,  10,   5,  10,  10,  20,   5,  10,  10,  10,  10 },
    /* PSN  */  { 10,  10,  10,  10,   5,   5,   5,  10,   5,   0,  10,  10,  20,  10,  10,  10,  10,  10 },
    /* GRD  */  { 10,  10,  10,   0,  20,  10,  20,   5,  10,  20,  20,  10,   5,  20,  10,  10,  10,  10 },
    /* RCK  */  { 10,  10,   5,  20,  10,   5,  10,  20,  10,   5,  20,  10,  10,  10,  10,  20,  10,  10 },
    /* BUG  */  { 10,  10,   5,   5,   5,  10,  10,  10,   5,   5,   5,  10,  20,  10,  20,  10,  10,  20 },
    /* GHO  */  { 10,   0,  10,  10,  10,  10,  10,  10,  20,   5,  10,  10,  10,  10,  20,  10,  10,   5 },
    /* STL  */  { 10,  10,  10,  10,  10,  10,  20,  10,  10,   5,   5,   5,  10,   5,  10,  20,  10,  10 },
    /* FIR  */  { 10,  10,  10,  10,  10,  10,   5,  20,  10,  20,   5,   5,  20,  10,  10,  20,   5,  10 },
    /* WTR  */  { 10,  10,  10,  10,  10,  20,  20,  10,  10,  10,  20,   5,   5,  10,  10,  10,   5,  10 },
    /* GRS  */  { 10,  10,  10,   5,   5,  20,  20,   5,  10,   5,   5,  20,   5,  10,  10,  10,   5,  10 },
    /* ELC  */  { 10,  10,  10,  20,  10,   0,  10,  10,  10,  10,  10,  20,   5,   5,  10,  10,   5,  10 },
    /* PSY  */  { 10,  10,  20,  10,  20,  10,  10,  10,  10,   5,  10,  10,  10,  10,   5,  10,  10,   0 },
    /* ICE  */  { 10,  10,  10,  20,  10,  20,  10,  10,  10,   5,   5,   5,  20,  10,  10,   5,  20,  10 },
    /* DRG  */  { 10,  10,  10,  10,  10,  10,  10,  10,  10,   5,  10,  10,  10,  10,  10,  10,  20,  10 },
    /* DRK  */  { 10,  10,   5,  10,  10,  10,  10,  10,  20,   5,  10,  10,  10,  10,  20,  10,  10,   5 },
};
// clang-format on

/**
 * @brief Get type effectiveness multiplier against a dual-typed defender.
 *
 * For mono-types, pass NONE as defend_type2.
 *
 * @pre attack_type must be a valid Type enum value (< TYPE_COUNT)
 * @pre defend_type1 must be a valid Type enum value (< TYPE_COUNT)
 * @pre defend_type2 must be a valid Type enum value (< TYPE_COUNT)
 *
 * @param attack_type The attacking move's type
 * @param defend_type1 The defender's primary type
 * @param defend_type2 The defender's secondary type (or Type::NONE if mono-type)
 *
 * @return Multiplier as (single_mult1 * single_mult2), where each single is 0/5/10/20.
 *   Common values:
 *   - 0   = immune (any 0 in the chain)
 *   - 25  = 0.25x (5 * 5, double resisted)
 *   - 50  = 0.5x  (5 * 10, resisted)
 *   - 100 = 1x    (10 * 10, neutral) = DUAL_NEUTRAL
 *   - 200 = 2x    (20 * 10, super effective)
 *   - 400 = 4x    (20 * 20, double super effective)
 */
constexpr Effectiveness get_type_effectiveness(types::enums::Type attack_type,
                                               types::enums::Type defend_type1,
                                               types::enums::Type defend_type2) {
    auto atk = static_cast<uint8_t>(attack_type);
    auto def1 = static_cast<uint8_t>(defend_type1);
    auto def2 = static_cast<uint8_t>(defend_type2);

    CONSTEXPR_ASSERT(atk < TYPE_COUNT);
    CONSTEXPR_ASSERT(def1 < TYPE_COUNT);
    CONSTEXPR_ASSERT(def2 < TYPE_COUNT);

    Effectiveness mult1 = TYPE_CHART[atk][def1];
    Effectiveness mult2 = TYPE_CHART[atk][def2];

    return mult1 * mult2;
}

// Check if the result is immune (0x)
constexpr bool is_immune(Effectiveness eff) {
    return eff == 0;
}

// Check if super effective (> DUAL_NEUTRAL after dual type calc)
constexpr bool is_super_effective(Effectiveness eff) {
    return eff > effectiveness::DUAL_NEUTRAL;
}

// Check if not very effective (< DUAL_NEUTRAL after dual type calc, but not immune)
constexpr bool is_not_very_effective(Effectiveness eff) {
    return eff > 0 && eff < effectiveness::DUAL_NEUTRAL;
}

}  // namespace logic::calc
