#pragma once

#include "base.hpp"
#include "types/enums/stats.hpp"

namespace logic::ops {

// ============================================================================
//                          STAT MODIFICATION
// ============================================================================
//
// ops for modifying stat stages (-6 to +6).
// Stages are stored as signed int8_t with 0 being neutral.
//
// Domain: Slot (writes stat stages)
// Stage:  Genesis -> EffectApplied (for stat-only moves)
//         DamageApplied -> EffectApplied (for secondary stat effects)
// ============================================================================

enum class Stat : uint8_t {
    ATK,
    DEF,
    SPD,
    SP_ATK,
    SP_DEF,
    ACCURACY,
    EVASION,
};

namespace detail {

// Helper to get reference to stat stage
inline int8_t& get_stage(logic::state::SlotState& slot, Stat stat) {
    switch (stat) {
        case Stat::ATK:
            return slot.atk_stage;
        case Stat::DEF:
            return slot.def_stage;
        case Stat::SPD:
            return slot.spd_stage;
        case Stat::SP_ATK:
            return slot.sp_atk_stage;
        case Stat::SP_DEF:
            return slot.sp_def_stage;
        case Stat::ACCURACY:
            return slot.accuracy_stage;
        case Stat::EVASION:
            return slot.evasion_stage;
    }
    return slot.atk_stage;  // Unreachable
}

}  // namespace detail

// ============================================================================
//                     MODIFY USER'S STAT (Genesis)
// ============================================================================

template <Stat S, int8_t Stages>
struct ModifyUserStat : CommandMeta<Domain::Slot, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        int8_t& stage = detail::get_stage(*ctx.attacker_slot, S);

        // Apply modification with bounds (-6 to +6)
        int8_t new_stage = stage + Stages;
        if (new_stage < -6)
            new_stage = -6;
        if (new_stage > 6)
            new_stage = 6;

        // Check if change happened (for "X's Y won't go any higher/lower!")
        if (new_stage == stage) {
            ctx.result.failed = true;
            return;
        }

        stage = new_stage;
    }
};

// ============================================================================
//                   MODIFY DEFENDER'S STAT (Genesis)
// ============================================================================

template <Stat S, int8_t Stages>
struct ModifyDefenderStat : CommandMeta<Domain::Slot, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        // TODO: Check for Mist protection
        // TODO: Check for abilities (Clear Body, White Smoke, etc.)

        int8_t& stage = detail::get_stage(*ctx.defender_slot, S);

        int8_t new_stage = stage + Stages;
        if (new_stage < -6)
            new_stage = -6;
        if (new_stage > 6)
            new_stage = 6;

        if (new_stage == stage) {
            ctx.result.failed = true;
            return;
        }

        stage = new_stage;
    }
};

// ============================================================================
//                   SECONDARY STAT EFFECTS (DamageApplied)
// ============================================================================

template <Stat S, int8_t Stages>
struct TryModifyDefenderStat : CommandMeta<Domain::Slot, DamageApplied, EffectApplied> {
    static void execute(dsl::BattleContext& ctx, uint8_t chance) {
        if (ctx.result.missed) {
            return;
        }

        // TODO: Roll for chance
        // For smoke testing, always apply
        if (chance == 0)
            return;

        int8_t& stage = detail::get_stage(*ctx.defender_slot, S);

        int8_t new_stage = stage + Stages;
        if (new_stage < -6)
            new_stage = -6;
        if (new_stage > 6)
            new_stage = 6;

        stage = new_stage;
    }
};

// ============================================================================
//                          CONVENIENT ALIASES
// ============================================================================

// User stat boosts (Swords Dance, Agility, etc.)
using RaiseUserAtk1 = ModifyUserStat<Stat::ATK, +1>;
using RaiseUserAtk2 = ModifyUserStat<Stat::ATK, +2>;
using RaiseUserDef1 = ModifyUserStat<Stat::DEF, +1>;
using RaiseUserDef2 = ModifyUserStat<Stat::DEF, +2>;
using RaiseUserSpd1 = ModifyUserStat<Stat::SPD, +1>;
using RaiseUserSpd2 = ModifyUserStat<Stat::SPD, +2>;
using RaiseUserSpAtk1 = ModifyUserStat<Stat::SP_ATK, +1>;
using RaiseUserSpAtk2 = ModifyUserStat<Stat::SP_ATK, +2>;
using RaiseUserSpDef1 = ModifyUserStat<Stat::SP_DEF, +1>;
using RaiseUserSpDef2 = ModifyUserStat<Stat::SP_DEF, +2>;
using RaiseUserEvasion1 = ModifyUserStat<Stat::EVASION, +1>;

// Defender stat drops (Growl, Leer, etc.)
using LowerDefenderAtk1 = ModifyDefenderStat<Stat::ATK, -1>;
using LowerDefenderAtk2 = ModifyDefenderStat<Stat::ATK, -2>;
using LowerDefenderDef1 = ModifyDefenderStat<Stat::DEF, -1>;
using LowerDefenderDef2 = ModifyDefenderStat<Stat::DEF, -2>;
using LowerDefenderSpd1 = ModifyDefenderStat<Stat::SPD, -1>;
using LowerDefenderSpd2 = ModifyDefenderStat<Stat::SPD, -2>;
using LowerDefenderSpAtk1 = ModifyDefenderStat<Stat::SP_ATK, -1>;
using LowerDefenderSpAtk2 = ModifyDefenderStat<Stat::SP_ATK, -2>;
using LowerDefenderSpDef1 = ModifyDefenderStat<Stat::SP_DEF, -1>;
using LowerDefenderSpDef2 = ModifyDefenderStat<Stat::SP_DEF, -2>;
using LowerDefenderAcc1 = ModifyDefenderStat<Stat::ACCURACY, -1>;
using LowerDefenderEva1 = ModifyDefenderStat<Stat::EVASION, -1>;

// Secondary effects (10% ATK drop from Aurora Beam, etc.)
using TryLowerDefenderAtk1 = TryModifyDefenderStat<Stat::ATK, -1>;
using TryLowerDefenderDef1 = TryModifyDefenderStat<Stat::DEF, -1>;
using TryLowerDefenderSpd1 = TryModifyDefenderStat<Stat::SPD, -1>;

// ============================================================================
//                          RESET ALL STATS (Haze)
// ============================================================================
//
// Resets all stat stages for both attacker and defender to neutral (0).
// In a full implementation, this would iterate over all battlers.
//
// Domain: Slot
// Stage:  Genesis -> EffectApplied
// ============================================================================

struct ResetAllStats : CommandMeta<Domain::Slot, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        // Reset attacker's stats
        reset_slot(*ctx.attacker_slot);

        // Reset defender's stats
        reset_slot(*ctx.defender_slot);

        // TODO: In doubles, iterate over all 4 slots
    }

   private:
    static void reset_slot(logic::state::SlotState& slot) {
        slot.atk_stage = 0;
        slot.def_stage = 0;
        slot.spd_stage = 0;
        slot.sp_atk_stage = 0;
        slot.sp_def_stage = 0;
        slot.accuracy_stage = 0;
        slot.evasion_stage = 0;
    }
};

}  // namespace logic::ops
