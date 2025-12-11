#pragma once

#include <cassert>

#include "util/random.hpp"

#include "types/calc.hpp"

namespace logic::calc {

using types::calc::CritStage;

// ============================================================================
//                           CRITICAL HITS
// ============================================================================
//
// Gen III critical hit mechanics.
//
// Critical hit stages (0-4) determine the 1/N chance of landing a crit.
// Each stage is contributed by different sources (Focus Energy, high-crit
// moves, held items like Scope Lens, species-specific items).
//
// Reference: pokeemerald/src/battle_script_commands.c Cmd_critcalc()
// ============================================================================

constexpr CritStage MAX_CRIT_STAGE = 4;

// Critical hit chance table: 1/N chance at each stage
// Stage 0 = 1/16, Stage 1 = 1/8, Stage 2 = 1/4, Stage 3 = 1/3, Stage 4 = 1/2
constexpr uint8_t CRIT_CHANCE[MAX_CRIT_STAGE + 1] = {16, 8, 4, 3, 2};

// Critical hit damage multiplier in Gen III
constexpr uint8_t CRIT_MULTIPLIER = 2;

/**
 * @brief Calculate the critical hit stage from various sources.
 *
 * Sources that contribute to crit stage:
 *   - Focus Energy: +2 stages
 *   - High-crit moves (Slash, etc.): +1 stage
 *   - Scope Lens: +1 stage
 *   - Lucky Punch (Chansey only): +2 stages
 *   - Stick/Leek (Farfetch'd only): +2 stages
 *
 * @param focus_energy Whether Focus Energy is active
 * @param high_crit_move Whether the move has high critical ratio
 * @param scope_lens Whether holding Scope Lens
 * @param species_bonus +2 for Chansey w/Lucky Punch or Farfetch'd w/Stick
 *
 * @return Critical hit stage (0-4, clamped)
 */
constexpr CritStage calc_crit_stage(bool focus_energy, bool high_crit_move,
                                     bool scope_lens, CritStage species_bonus = 0) {
    CritStage stage = 0;

    if (focus_energy) stage += 2;
    if (high_crit_move) stage += 1;
    if (scope_lens) stage += 1;
    stage += species_bonus;

    if (stage > MAX_CRIT_STAGE) stage = MAX_CRIT_STAGE;
    return stage;
}

/**
 * @brief Roll for a critical hit.
 *
 * @pre crit_stage must be in range [0, 4]
 *
 * @param crit_stage The calculated critical hit stage
 *
 * @return true if the hit is critical
 */
inline bool roll_critical(CritStage crit_stage) {
    assert(crit_stage <= MAX_CRIT_STAGE && "crit_stage out of range");

    uint16_t threshold = CRIT_CHANCE[crit_stage];
    return util::random::Random(threshold) == 0;
}

}  // namespace logic::calc
