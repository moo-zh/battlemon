#pragma once

#include "util/assert.hpp"

#include "types/calc.hpp"

namespace logic::calc {

using types::calc::StatStage;
using types::calc::StatValue;

// ============================================================================
//                         STAT STAGE MULTIPLIERS
// ============================================================================
//
// Gen III stat stage modifier table.
//
// Stat stages range from -6 to +6, stored as 0-12 (with 6 being neutral).
// Each stage modifies the stat by a ratio: (numerator / denominator).
//
// Reference: pokeemerald/src/pokemon.c gStatStageRatios[]
// ============================================================================

constexpr StatStage MIN_STAT_STAGE = 0;      // -6
constexpr StatStage DEFAULT_STAT_STAGE = 6;  // neutral
constexpr StatStage MAX_STAT_STAGE = 12;     // +6

// Stat stage ratios [stage][0=numerator, 1=denominator]
// Stage 0 = -6, Stage 6 = neutral, Stage 12 = +6
//
// clang-format off
constexpr uint8_t STAT_STAGE_RATIOS[MAX_STAT_STAGE + 1][2] = {
    {10, 40},  // -6: 10/40 = 0.25x
    {10, 35},  // -5: 10/35 ≈ 0.29x
    {10, 30},  // -4: 10/30 ≈ 0.33x
    {10, 25},  // -3: 10/25 = 0.40x
    {10, 20},  // -2: 10/20 = 0.50x
    {10, 15},  // -1: 10/15 ≈ 0.67x
    {10, 10},  //  0: 10/10 = 1.00x (neutral)
    {15, 10},  // +1: 15/10 = 1.50x
    {20, 10},  // +2: 20/10 = 2.00x
    {25, 10},  // +3: 25/10 = 2.50x
    {30, 10},  // +4: 30/10 = 3.00x
    {35, 10},  // +5: 35/10 = 3.50x
    {40, 10},  // +6: 40/10 = 4.00x
};
// clang-format on

/**
 * @brief Apply stat stage modifier to a base stat value.
 *
 * @pre stage must be in range [0, 12]
 *
 * @param base_stat The unmodified stat value
 * @param stage The stat stage (0-12, where 6 is neutral)
 *
 * @return The modified stat value after applying stage multiplier
 */
constexpr StatValue apply_stat_stage(StatValue base_stat, StatStage stage) {
    CONSTEXPR_ASSERT(stage <= MAX_STAT_STAGE);

    uint32_t result = base_stat;
    result *= STAT_STAGE_RATIOS[stage][0];
    result /= STAT_STAGE_RATIOS[stage][1];

    return static_cast<StatValue>(result);
}

/**
 * @brief Clamp a stat stage delta to valid range.
 *
 * @param current Current stage value (0-12)
 * @param delta Change to apply (can be negative)
 *
 * @return New stage value clamped to [0, 12]
 */
constexpr StatStage clamp_stat_stage(StatStage current, int8_t delta) {
    int16_t result = static_cast<int16_t>(current) + delta;
    if (result < MIN_STAT_STAGE)
        return MIN_STAT_STAGE;
    if (result > MAX_STAT_STAGE)
        return MAX_STAT_STAGE;
    return static_cast<StatStage>(result);
}

}  // namespace logic::calc
