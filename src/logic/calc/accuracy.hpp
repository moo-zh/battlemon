#pragma once

#include <cstdint>

#include "types/calc.hpp"
#include "util/random.hpp"

namespace logic::calc {

// ============================================================================
//                           ACCURACY CALCULATION
// ============================================================================
//
// Gen III accuracy mechanics.
//
// Effective accuracy formula:
//   accuracy = move_accuracy * (attacker_acc_modifier / defender_eva_modifier)
//
// Stage modifiers (Gen III uses the same table for accuracy and evasion):
//   Stage -6: 33/100 (3/9)
//   Stage -5: 36/100 (3/8)
//   Stage -4: 43/100 (3/7)
//   Stage -3: 50/100 (3/6)
//   Stage -2: 60/100 (3/5)
//   Stage -1: 75/100 (3/4)
//   Stage  0: 100/100 (3/3)
//   Stage +1: 133/100 (4/3)
//   Stage +2: 166/100 (5/3)
//   Stage +3: 200/100 (6/3)
//   Stage +4: 233/100 (7/3)
//   Stage +5: 266/100 (8/3)
//   Stage +6: 300/100 (9/3)
//
// Stages are stored as signed int8_t (-6 to +6, 0 = neutral).
//
// Reference: pokeemerald/src/battle_util.c AccuracyCalc()
// ============================================================================

// Accuracy/Evasion stage multipliers (indexed by stage + 6)
// Using integer math: multiply by numerator, divide by denominator
constexpr uint8_t ACC_STAGE_NUMERATORS[13] = {3, 3, 3, 3, 3, 3, 3, 4, 5, 6, 7, 8, 9};
constexpr uint8_t ACC_STAGE_DENOMINATORS[13] = {9, 8, 7, 6, 5, 4, 3, 3, 3, 3, 3, 3, 3};

/**
 * @brief Convert signed stage (-6 to +6) to array index (0 to 12).
 */
constexpr size_t acc_stage_to_index(int8_t stage) {
    // Clamp to [-6, 6] range
    if (stage < -6)
        stage = -6;
    if (stage > 6)
        stage = 6;
    return static_cast<size_t>(stage + 6);
}

/**
 * @brief Calculate effective accuracy considering stat stages.
 *
 * @param base_accuracy Move's base accuracy (1-100, or 0 for never-miss)
 * @param acc_stage Attacker's accuracy stage (-6 to +6)
 * @param eva_stage Defender's evasion stage (-6 to +6)
 *
 * @return Effective accuracy percentage (capped at 100)
 */
constexpr uint8_t calc_effective_accuracy(uint8_t base_accuracy, int8_t acc_stage,
                                          int8_t eva_stage) {
    if (base_accuracy == 0) {
        // Accuracy 0 means "never miss" (Swift, Aerial Ace, etc.)
        return 100;
    }

    uint32_t accuracy = base_accuracy;

    // Apply accuracy stage modifier (attacker's buff/debuff)
    if (acc_stage != 0) {
        size_t idx = acc_stage_to_index(acc_stage);
        accuracy = accuracy * ACC_STAGE_NUMERATORS[idx] / ACC_STAGE_DENOMINATORS[idx];
    }

    // Apply evasion stage modifier (defender's buff/debuff)
    // Evasion reduces hit chance, so we use inverse multiplier
    if (eva_stage != 0) {
        size_t idx = acc_stage_to_index(eva_stage);
        // Inverse: divide by numerator, multiply by denominator
        accuracy = accuracy * ACC_STAGE_DENOMINATORS[idx] / ACC_STAGE_NUMERATORS[idx];
    }

    // Cap at 100%
    return static_cast<uint8_t>(accuracy > 100 ? 100 : accuracy);
}

/**
 * @brief Roll for move accuracy.
 *
 * @param effective_accuracy Calculated effective accuracy (1-100)
 *
 * @return true if the move hits, false if it misses
 */
inline bool roll_accuracy(uint8_t effective_accuracy) {
    if (effective_accuracy >= 100) {
        // Still consume the RNG call for parity with Showdown
        util::random::Random(100);
        return true;
    }

    // Hit if random(100) < effective_accuracy
    uint16_t roll = util::random::Random(100);
    return roll < effective_accuracy;
}

/**
 * @brief Check if a move with given accuracy hits.
 *
 * Combines accuracy calculation and roll into a single function.
 *
 * @param base_accuracy Move's base accuracy (0 = never miss, 1-100 = percentage)
 * @param acc_stage Attacker's accuracy stage (-6 to +6)
 * @param eva_stage Defender's evasion stage (-6 to +6)
 *
 * @return true if the move hits
 */
inline bool check_accuracy(uint8_t base_accuracy, int8_t acc_stage = 0, int8_t eva_stage = 0) {
    // Never-miss moves (accuracy == 0) always hit but still consume RNG
    if (base_accuracy == 0) {
        // Don't consume RNG for never-miss moves (Showdown doesn't either)
        return true;
    }

    uint8_t effective = calc_effective_accuracy(base_accuracy, acc_stage, eva_stage);
    return roll_accuracy(effective);
}

}  // namespace logic::calc
