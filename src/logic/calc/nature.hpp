#pragma once

#include "types/calc.hpp"
#include "types/enums/nature.hpp"
#include "types/enums/stats.hpp"
#include "util/assert.hpp"

namespace logic::calc {

using types::calc::NatureModifier;
using types::calc::StatValue;

// ============================================================================
//                           NATURE MODIFIERS
// ============================================================================
//
// Gen III nature mechanics affect 4 stats: Attack, Defense, Speed, Sp.Atk,
// Sp.Def. HP is never affected. Each nature either:
//   - Neutral (no effect on any stat)
//   - +10% to one stat, -10% to another
//
// The table is indexed by [Nature][StatIndex] where StatIndex is:
//   0 = Attack, 1 = Defense, 2 = Speed, 3 = Sp.Atk, 4 = Sp.Def
//
// Values: +1 = boosted (110%), -1 = lowered (90%), 0 = neutral (100%)
//
// Reference: pokeemerald/src/pokemon.c gNatureStatTable[]
// ============================================================================

// Number of stats affected by nature (excludes HP)
constexpr uint8_t NUM_NATURE_STATS = 5;
constexpr uint8_t NUM_NATURES = 25;

// Nature stat modifier table: [nature][stat_index] -> {-1, 0, +1}
// Stat indices: 0=Atk, 1=Def, 2=Spd, 3=SpAtk, 4=SpDef
// Ordered to match types::enums::Nature (alphabetical)
constexpr int8_t NATURE_STAT_TABLE[NUM_NATURES][NUM_NATURE_STATS] = {
    // clang-format off
    //                   Atk  Def  Spd SpAtk SpDef
    /* ADAMANT */     {  +1,   0,   0,   -1,   0  },
    /* BASHFUL */     {   0,   0,   0,    0,   0  },
    /* BOLD    */     {  -1,  +1,   0,    0,   0  },
    /* BRAVE   */     {  +1,   0,  -1,    0,   0  },
    /* CALM    */     {  -1,   0,   0,    0,  +1  },
    /* CAREFUL */     {   0,   0,   0,   -1,  +1  },
    /* DOCILE  */     {   0,   0,   0,    0,   0  },
    /* GENTLE  */     {   0,  -1,   0,    0,  +1  },
    /* HARDY   */     {   0,   0,   0,    0,   0  },
    /* HASTY   */     {   0,  -1,  +1,    0,   0  },
    /* IMPISH  */     {   0,  +1,   0,   -1,   0  },
    /* JOLLY   */     {   0,   0,  +1,   -1,   0  },
    /* LAX     */     {   0,  +1,   0,    0,  -1  },
    /* LONELY  */     {  +1,  -1,   0,    0,   0  },
    /* MILD    */     {   0,  -1,   0,   +1,   0  },
    /* MODEST  */     {  -1,   0,   0,   +1,   0  },
    /* NAIVE   */     {   0,   0,  +1,    0,  -1  },
    /* NAUGHTY */     {  +1,   0,   0,    0,  -1  },
    /* QUIET   */     {   0,   0,  -1,   +1,   0  },
    /* QUIRKY  */     {   0,   0,   0,    0,   0  },
    /* RASH    */     {   0,   0,   0,   +1,  -1  },
    /* RELAXED */     {   0,  +1,  -1,    0,   0  },
    /* SASSY   */     {   0,   0,  -1,    0,  +1  },
    /* SERIOUS */     {   0,   0,   0,    0,   0  },
    /* TIMID   */     {  -1,   0,  +1,    0,   0  },
    // clang-format on
};

/**
 * @brief Get the nature modifier for a specific stat.
 *
 * @pre nature must be a valid Nature enum value (0-24)
 * @pre stat_index must be in range [0, 4] (Atk, Def, Spd, SpAtk, SpDef)
 *
 * @param nature The pokemon's nature
 * @param stat_index Index of stat (0=Atk, 1=Def, 2=Spd, 3=SpAtk, 4=SpDef)
 *
 * @return -1 for lowered, 0 for neutral, +1 for boosted
 */
constexpr NatureModifier get_nature_modifier(types::enums::Nature nature, uint8_t stat_index) {
    CONSTEXPR_ASSERT(static_cast<uint8_t>(nature) < NUM_NATURES);
    CONSTEXPR_ASSERT(stat_index < NUM_NATURE_STATS);

    return NATURE_STAT_TABLE[static_cast<uint8_t>(nature)][stat_index];
}

/**
 * @brief Apply nature modifier to a calculated stat value.
 *
 * Nature multipliers:
 *   +1 -> 110% (multiply by 11, divide by 10)
 *   -1 -> 90%  (multiply by 9, divide by 10)
 *    0 -> 100% (no change)
 *
 * @pre nature must be a valid Nature enum value (0-24)
 * @pre stat_index must be in range [0, 4] (Atk, Def, Spd, SpAtk, SpDef)
 *
 * @param stat The base stat value (after level/IV/EV calculation)
 * @param nature The pokemon's nature
 * @param stat_index Index of stat (0=Atk, 1=Def, 2=Spd, 3=SpAtk, 4=SpDef)
 *
 * @return The stat modified by nature
 */
constexpr StatValue apply_nature(StatValue stat, types::enums::Nature nature, uint8_t stat_index) {
    NatureModifier modifier = get_nature_modifier(nature, stat_index);

    if (modifier > 0) {
        return static_cast<StatValue>(stat * 11u / 10u);
    } else if (modifier < 0) {
        return static_cast<StatValue>(stat * 9u / 10u);
    }
    return stat;
}

}  // namespace logic::calc
