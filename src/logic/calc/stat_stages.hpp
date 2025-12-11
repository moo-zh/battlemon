#pragma once

#include "types/calc.hpp"
#include "util/assert.hpp"

namespace logic::calc {

using types::calc::StatStage;
using types::calc::StatValue;

// ============================================================================
//                         STAT STAGE MULTIPLIERS
// ============================================================================
//
// Gen III stat stage modifier table.
//
// Stat stages range from -6 to +6, with 0 being neutral.
// Each stage modifies the stat by a ratio: (numerator / denominator).
//
// Reference: pokeemerald/src/pokemon.c gStatStageRatios[]
// ============================================================================

constexpr int8_t MIN_STAT_STAGE = -6;
constexpr int8_t DEFAULT_STAT_STAGE = 0;
constexpr int8_t MAX_STAT_STAGE = 6;

/**
 * @brief Convert signed stage (-6 to +6) to array index (0 to 12).
 */
constexpr size_t stage_to_index(int8_t stage) {
    return static_cast<size_t>(stage + 6);
}

// Stat stage ratios [index][0=numerator, 1=denominator]
// Index 0 = stage -6, Index 6 = neutral, Index 12 = stage +6
//
// clang-format off
constexpr uint8_t STAT_STAGE_RATIOS[13][2] = {
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
 * @pre stage must be in range [-6, 6]
 *
 * @param base_stat The unmodified stat value
 * @param stage The stat stage (-6 to +6, where 0 is neutral)
 *
 * @return The modified stat value after applying stage multiplier
 */
constexpr StatValue apply_stat_stage(StatValue base_stat, int8_t stage) {
    CONSTEXPR_ASSERT(stage >= MIN_STAT_STAGE && stage <= MAX_STAT_STAGE);

    size_t idx = stage_to_index(stage);
    uint32_t result = base_stat;
    result *= STAT_STAGE_RATIOS[idx][0];
    result /= STAT_STAGE_RATIOS[idx][1];

    return static_cast<StatValue>(result);
}

/**
 * @brief Clamp a stat stage delta to valid range.
 *
 * @param current Current stage value (-6 to +6)
 * @param delta Change to apply (can be negative)
 *
 * @return New stage value clamped to [-6, +6]
 */
constexpr int8_t clamp_stat_stage(int8_t current, int8_t delta) {
    int16_t result = static_cast<int16_t>(current) + delta;
    if (result < MIN_STAT_STAGE)
        return MIN_STAT_STAGE;
    if (result > MAX_STAT_STAGE)
        return MAX_STAT_STAGE;
    return static_cast<int8_t>(result);
}

}  // namespace logic::calc
