#pragma once

#include "util/assert.hpp"

#include "nature.hpp"
#include "types/calc.hpp"
#include "types/enums/nature.hpp"
#include "types/enums/species.hpp"
#include "types/enums/stats.hpp"

namespace logic::calc {

using types::calc::BaseStat;
using types::calc::EV;
using types::calc::IV;
using types::calc::Level;
using types::calc::StatValue;

// ============================================================================
//                         GEN III STAT FORMULAS
// ============================================================================
//
// Pokemon stats are calculated from base stats, IVs, EVs, level, and nature.
//
// HP Formula:
//   floor((2 * Base + IV + floor(EV / 4)) * Level / 100) + Level + 10
//   Exception: Shedinja always has 1 HP
//
// Other Stats (Atk, Def, SpAtk, SpDef, Spd):
//   floor(floor((2 * Base + IV + floor(EV / 4)) * Level / 100 + 5) * NatureMod)
//   Where NatureMod is 1.1 (+nature), 0.9 (-nature), or 1.0 (neutral)
//
// Battle Factory specifics:
//   - Level is fixed at 50 (or 100 in Open Level)
//   - IVs are typically 31 across the board
//   - EVs are 510 total, distributed 252/252/6 or similar based on evSpread
//
// Reference: pokeemerald/src/pokemon.c CalculateMonStats()
// ============================================================================

// Battle Factory uses either level 50 or level 100
constexpr Level FACTORY_LEVEL_50 = 50;
constexpr Level FACTORY_LEVEL_100 = 100;

// Standard IV value for Battle Factory mons
constexpr IV FACTORY_IV = 31;

// EV distribution constants
constexpr uint16_t MAX_EV_PER_STAT = 252;
constexpr uint16_t MAX_TOTAL_EVS = 510;

// Stat indices for nature modifier (excludes HP)
namespace stat_index {
constexpr uint8_t ATK = 0;
constexpr uint8_t DEF = 1;
constexpr uint8_t SPD = 2;
constexpr uint8_t SP_ATK = 3;
constexpr uint8_t SP_DEF = 4;
}  // namespace stat_index

/**
 * @brief Calculate HP stat from components.
 *
 * Formula: floor((2 * base + iv + floor(ev / 4)) * level / 100) + level + 10
 *
 * @pre level must be > 0
 *
 * @param base Base HP stat
 * @param iv Individual value (0-31)
 * @param ev Effort value (0-255)
 * @param level Pokemon level (1-100)
 *
 * @return Calculated HP stat
 */
constexpr StatValue calc_hp(BaseStat base, IV iv, EV ev, Level level) {
    CONSTEXPR_ASSERT(level > 0);

    uint32_t stat = 2u * base + iv + ev / 4u;
    stat = stat * level / 100u;
    stat += level + 10u;

    return static_cast<StatValue>(stat);
}

/**
 * @brief Calculate non-HP stat (Atk, Def, SpAtk, SpDef, Spd) from components.
 *
 * Formula: floor((floor((2 * base + iv + floor(ev / 4)) * level / 100) + 5) * nature_mod)
 *
 * @pre level must be > 0
 * @pre stat_index must be in range [0, 4]
 *
 * @param base Base stat value
 * @param iv Individual value (0-31)
 * @param ev Effort value (0-255)
 * @param level Pokemon level (1-100)
 * @param nature Pokemon's nature
 * @param stat_idx Stat index for nature lookup (0=Atk, 1=Def, 2=Spd, 3=SpAtk, 4=SpDef)
 *
 * @return Calculated stat value
 */
constexpr StatValue calc_stat(BaseStat base, IV iv, EV ev, Level level,
                               types::enums::Nature nature, uint8_t stat_idx) {
    CONSTEXPR_ASSERT(level > 0);
    CONSTEXPR_ASSERT(stat_idx < NUM_NATURE_STATS);

    uint32_t stat = 2u * base + iv + ev / 4u;
    stat = stat * level / 100u + 5u;

    // Apply nature modifier
    return apply_nature(static_cast<StatValue>(stat), nature, stat_idx);
}

// ============================================================================
//                         STAT BLOCK CALCULATION
// ============================================================================

/**
 * @brief Complete stat block for a Pokemon.
 */
struct StatBlock {
    StatValue hp;
    StatValue attack;
    StatValue defense;
    StatValue sp_attack;
    StatValue sp_defense;
    StatValue speed;
};

/**
 * @brief Base stats array (indexed by BaseStat enum).
 */
struct BaseStats {
    BaseStat stats[6];

    constexpr BaseStat hp() const { return stats[0]; }
    constexpr BaseStat atk() const { return stats[1]; }
    constexpr BaseStat def() const { return stats[2]; }
    constexpr BaseStat sp_atk() const { return stats[3]; }
    constexpr BaseStat sp_def() const { return stats[4]; }
    constexpr BaseStat spd() const { return stats[5]; }
};

/**
 * @brief IV spread (typically all 31 for Battle Factory).
 */
struct IVSpread {
    IV hp{31};
    IV atk{31};
    IV def{31};
    IV sp_atk{31};
    IV sp_def{31};
    IV spd{31};

    static constexpr IVSpread perfect() { return {31, 31, 31, 31, 31, 31}; }
};

/**
 * @brief EV spread (0-255 per stat, max 510 total).
 */
struct EVSpread {
    EV hp{0};
    EV atk{0};
    EV def{0};
    EV sp_atk{0};
    EV sp_def{0};
    EV spd{0};
};

/**
 * @brief Calculate complete stat block from components.
 *
 * @param base Base stats for the species
 * @param ivs Individual values
 * @param evs Effort values
 * @param level Pokemon level
 * @param nature Pokemon's nature
 * @param is_shedinja If true, HP is always 1
 *
 * @return Complete calculated stat block
 */
constexpr StatBlock calc_stats(const BaseStats& base, const IVSpread& ivs,
                               const EVSpread& evs, Level level,
                               types::enums::Nature nature, bool is_shedinja = false) {
    StatBlock result{};

    // HP (special case for Shedinja)
    if (is_shedinja) {
        result.hp = 1;
    } else {
        result.hp = calc_hp(base.hp(), ivs.hp, evs.hp, level);
    }

    // Other stats with nature modifiers
    result.attack = calc_stat(base.atk(), ivs.atk, evs.atk, level, nature, stat_index::ATK);
    result.defense = calc_stat(base.def(), ivs.def, evs.def, level, nature, stat_index::DEF);
    result.sp_attack = calc_stat(base.sp_atk(), ivs.sp_atk, evs.sp_atk, level, nature, stat_index::SP_ATK);
    result.sp_defense = calc_stat(base.sp_def(), ivs.sp_def, evs.sp_def, level, nature, stat_index::SP_DEF);
    result.speed = calc_stat(base.spd(), ivs.spd, evs.spd, level, nature, stat_index::SPD);

    return result;
}

}  // namespace logic::calc
