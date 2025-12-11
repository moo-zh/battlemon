#pragma once

#include <cassert>

#include "../calc/stats.hpp"
#include "../state/context.hpp"
#include "../state/mon.hpp"
#include "../state/slot.hpp"

#include "data/rental.hpp"
#include "data/species.hpp"
#include "types/models/rental.hpp"
#include "types/models/species.hpp"

namespace logic::setup {

// ============================================================================
//                         RENTAL -> BATTLE SETUP
// ============================================================================
//
// Converts Battle Factory rental data into battle-ready state structures.
//
// A Rental contains:
//   - Species (enum -> lookup in g_SPECIES_TABLE for base stats, types)
//   - Moves[4]
//   - Held item
//   - Nature (affects stat calculation)
//   - EV spread (6-bit flags for which stats get 252 EVs)
//   - Ability slot (0 or 1)
//
// This produces:
//   - MonState: runtime HP, status, etc.
//   - SlotState: battle position state (stat stages, volatiles)
//   - ActiveMon: computed stats for damage calculation
//
// ============================================================================

// Battle Factory IV value (all Pokemon have perfect IVs)
constexpr uint8_t RENTAL_IV = 31;

/**
 * @brief Convert rental EV spread flags to full EV values.
 *
 * Battle Factory distributes 510 total EVs evenly among flagged stats.
 * The EV amount per stat is: 510 / (number of flagged stats)
 *
 * Examples:
 *   - 2 stats flagged: 510/2 = 255 each (capped at 252)
 *   - 3 stats flagged: 510/3 = 170 each
 *
 * Reference: pokeemerald/src/pokemon.c CreateMonWithEVSpread()
 *
 * @param spread The packed EV spread bits from Rental
 *
 * @return Full EVSpread struct with calculated EVs per flagged stat
 */
constexpr calc::EVSpread unpack_ev_spread(types::EvSpread spread) {
    calc::EVSpread evs{};

    // Count flagged stats
    uint8_t stat_count = 0;
    if (spread.hp()) ++stat_count;
    if (spread.atk()) ++stat_count;
    if (spread.def()) ++stat_count;
    if (spread.sp_atk()) ++stat_count;
    if (spread.sp_def()) ++stat_count;
    if (spread.spd()) ++stat_count;

    if (stat_count == 0) {
        return evs;  // No EVs allocated
    }

    // Distribute 510 EVs evenly among flagged stats
    // Note: MAX_TOTAL_EVS = 510, but individual stat cap is 255 (uint8_t max)
    uint16_t ev_amount = calc::MAX_TOTAL_EVS / stat_count;
    if (ev_amount > 255) ev_amount = 255;  // Cap to uint8_t max

    uint8_t ev_val = static_cast<uint8_t>(ev_amount);
    if (spread.hp()) evs.hp = ev_val;
    if (spread.atk()) evs.atk = ev_val;
    if (spread.def()) evs.def = ev_val;
    if (spread.sp_atk()) evs.sp_atk = ev_val;
    if (spread.sp_def()) evs.sp_def = ev_val;
    if (spread.spd()) evs.spd = ev_val;

    return evs;
}

/**
 * @brief Look up species data from the global species table.
 *
 * @pre species must be a valid species enum with an entry in g_SPECIES_TABLE
 *
 * @param species The species enum to look up
 *
 * @return Pointer to the species data
 */
inline const types::Species* lookup_species(types::enums::Species species) {
    // The species table is indexed directly by species enum value
    auto index = static_cast<uint16_t>(species);
    assert(index < sizeof(data::g_SPECIES_TABLE) / sizeof(data::g_SPECIES_TABLE[0])
           && "species index out of bounds");
    return &data::g_SPECIES_TABLE[index];
}

/**
 * @brief Result of setting up a rental Pokemon for battle.
 */
struct RentalSetup {
    logic::state::MonState mon;
    logic::state::SlotState slot;
    dsl::ActiveMon active;

    // Additional info that might be needed
    types::enums::Ability ability;
};

/**
 * @brief Set up a rental Pokemon for battle.
 *
 * Converts Rental data into battle-ready state structures with
 * calculated stats and initialized state.
 *
 * @param rental The rental Pokemon data
 * @param level Battle level (50 for Level 50, 100 for Open Level)
 *
 * @return RentalSetup containing mon, slot, and active mon data
 */
inline RentalSetup setup_rental(const types::Rental& rental, uint8_t level = 50) {
    RentalSetup result{};

    // Look up species for base stats and types
    const types::Species* species = lookup_species(rental.species);
    assert(species && "species lookup failed");

    // Build base stats struct from species data
    calc::BaseStats base{};
    for (int i = 0; i < 6; ++i) {
        base.stats[i] = species->stats[i];
    }

    // Perfect IVs for Battle Factory
    calc::IVSpread ivs = calc::IVSpread::perfect();

    // Unpack EV spread
    calc::EVSpread evs = unpack_ev_spread(rental.ev_spread);

    // Check for Shedinja (always 1 HP)
    bool is_shedinja = (rental.species == types::enums::Species::SHEDINJA);

    // Calculate stats
    calc::StatBlock stats = calc::calc_stats(base, ivs, evs, level, rental.nature, is_shedinja);

    // Initialize MonState
    result.mon.max_hp = stats.hp;
    result.mon.current_hp = stats.hp;
    result.mon.status = logic::state::Status::NONE;

    // Initialize SlotState (all stages at neutral)
    result.slot = logic::state::SlotState{};  // Default constructor sets neutral stages

    // Initialize ActiveMon for damage calculation
    result.active.level = level;
    result.active.attack = stats.attack;
    result.active.defense = stats.defense;
    result.active.sp_attack = stats.sp_attack;
    result.active.sp_defense = stats.sp_defense;
    result.active.speed = stats.speed;
    result.active.type1 = species->type1;
    result.active.type2 = species->type2;

    // Determine ability based on slot
    result.ability = (rental.ability_slot == 0) ? species->ability1 : species->ability2;
    // Fall back to ability1 if ability2 is NONE
    if (result.ability == types::enums::Ability::NONE) {
        result.ability = species->ability1;
    }

    return result;
}

/**
 * @brief Set up two rentals for a battle and populate a BattleContext.
 *
 * @param ctx The battle context to populate
 * @param attacker_rental Attacker's rental data
 * @param defender_rental Defender's rental data
 * @param attacker_setup Output: attacker's setup data (caller manages lifetime)
 * @param defender_setup Output: defender's setup data (caller manages lifetime)
 * @param level Battle level (default 50)
 */
inline void setup_battle(dsl::BattleContext& ctx,
                         const types::Rental& attacker_rental,
                         const types::Rental& defender_rental,
                         RentalSetup& attacker_setup,
                         RentalSetup& defender_setup,
                         uint8_t level = 50) {
    attacker_setup = setup_rental(attacker_rental, level);
    defender_setup = setup_rental(defender_rental, level);

    // Wire up context pointers
    ctx.attacker_mon = &attacker_setup.mon;
    ctx.defender_mon = &defender_setup.mon;
    ctx.attacker_slot = &attacker_setup.slot;
    ctx.defender_slot = &defender_setup.slot;
    ctx.attacker_active = &attacker_setup.active;
    ctx.defender_active = &defender_setup.active;

    // Set up slots array for iteration
    ctx.slots[0] = &attacker_setup.slot;
    ctx.slots[1] = &defender_setup.slot;
    ctx.mons[0] = &attacker_setup.mon;
    ctx.mons[1] = &defender_setup.mon;
    ctx.active_slot_count = 2;
}

}  // namespace logic::setup
