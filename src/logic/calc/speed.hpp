#pragma once

#include "logic/state/context.hpp"
#include "logic/state/mon.hpp"
#include "logic/state/slot.hpp"
#include "stat_stages.hpp"
#include "types/calc.hpp"

namespace logic::calc {

using types::calc::StatValue;

// ============================================================================
//                           EFFECTIVE SPEED
// ============================================================================
//
// Gen III effective speed calculation for turn order determination.
//
// Formula:
//   effective_speed = base_speed * stage_modifier
//   If paralyzed: effective_speed /= 4
//
// Note: Weather abilities (Swift Swim, Chlorophyll) and items (Quick Claw,
// Macho Brace) are NOT implemented for Battle Factory Milestone 1.
//
// Reference: pokeemerald/src/battle_main.c GetWhoStrikesFirst()
// ============================================================================

/**
 * @brief Calculate effective speed for turn order determination.
 *
 * Applies:
 *   1. Speed stat stage modifier
 *   2. Paralysis penalty (/4)
 *
 * @param base_speed The pre-calculated speed stat (with IVs/EVs/nature)
 * @param speed_stage The current speed stage (0-12, 6 = neutral)
 * @param status The mon's current status condition
 *
 * @return Effective speed value for turn order comparison
 */
constexpr StatValue calc_effective_speed(StatValue base_speed, StatStage speed_stage,
                                         logic::state::Status status) {
    // Apply speed stage modifier
    StatValue speed = apply_stat_stage(base_speed, speed_stage);

    // Paralysis quarters speed
    if (status == logic::state::Status::PARALYSIS) {
        speed /= 4;
    }

    return speed;
}

/**
 * @brief Calculate effective speed from battle state objects.
 *
 * Convenience overload that extracts data from ActiveMon, SlotState, and MonState.
 */
constexpr StatValue calc_effective_speed(const dsl::ActiveMon& active,
                                         const logic::state::SlotState& slot,
                                         const logic::state::MonState& mon) {
    return calc_effective_speed(active.speed, static_cast<StatStage>(slot.spd_stage), mon.status);
}

// ============================================================================
//                            TURN ORDER
// ============================================================================
//
// Turn order rules (Gen III):
//   1. Higher priority bracket goes first
//   2. Within same priority, higher effective speed goes first
//   3. Speed tie: 50/50 random
//
// Return value:
//   0 = battler1 goes first
//   1 = battler2 goes first
//   2 = tie (caller should randomize)
// ============================================================================

enum class TurnOrder : uint8_t {
    BATTLER1_FIRST = 0,
    BATTLER2_FIRST = 1,
    SPEED_TIE = 2,
};

/**
 * @brief Determine turn order between two battlers.
 *
 * @param priority1 Move priority for battler 1 (-7 to +5)
 * @param priority2 Move priority for battler 2 (-7 to +5)
 * @param speed1 Effective speed of battler 1
 * @param speed2 Effective speed of battler 2
 *
 * @return TurnOrder indicating who goes first or if there's a tie
 */
constexpr TurnOrder determine_turn_order(int8_t priority1, int8_t priority2, StatValue speed1,
                                         StatValue speed2) {
    // Different priorities - higher priority goes first
    if (priority1 != priority2) {
        return (priority1 > priority2) ? TurnOrder::BATTLER1_FIRST : TurnOrder::BATTLER2_FIRST;
    }

    // Same priority - compare effective speeds
    if (speed1 > speed2) {
        return TurnOrder::BATTLER1_FIRST;
    }
    if (speed2 > speed1) {
        return TurnOrder::BATTLER2_FIRST;
    }

    // Speed tie - caller must randomize
    return TurnOrder::SPEED_TIE;
}

}  // namespace logic::calc
