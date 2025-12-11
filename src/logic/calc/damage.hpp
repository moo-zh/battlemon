#pragma once

#include "critical.hpp"
#include "stat_stages.hpp"
#include "type_effectiveness.hpp"
#include "types/calc.hpp"
#include "types/enums/type.hpp"
#include "util/random.hpp"

namespace logic::calc {

using types::calc::CritStage;
using types::calc::Damage;
using types::calc::DamageCalc;
using types::calc::Effectiveness;
using types::calc::Level;
using types::calc::MovePower;
using types::calc::StatStage;
using types::calc::StatValue;

// ============================================================================
//                         GEN III DAMAGE FORMULA
// ============================================================================
//
// Base damage formula:
//   ((2 * Level / 5 + 2) * Power * Attack / Defense) / 50 + 2
//
// Then multiply by (in order):
//   1. Critical hit (2x)
//   2. STAB (1.5x if attacker shares type with move)
//   3. Type effectiveness (0x, 0.25x, 0.5x, 1x, 2x, 4x)
//   4. Random factor (85-100%) - applied last per Gen III behavior
//
// Reference: pokeemerald/src/pokemon.c CalculateBaseDamage()
//            pokeemerald/src/battle_script_commands.c Cmd_typecalc()
// ============================================================================

// Result of damage calculation
struct DamageResult {
    Damage damage{0};
    Effectiveness effectiveness{effectiveness::DUAL_NEUTRAL};
    bool critical{false};
};

// Input parameters for damage calculation
struct DamageParams {
    // Attacker info
    Level level{50};
    StatValue attack{100};  // Atk or SpAtk depending on move
    StatStage attack_stage{DEFAULT_STAT_STAGE};
    types::enums::Type attacker_type1{types::enums::Type::NONE};
    types::enums::Type attacker_type2{types::enums::Type::NONE};

    // Defender info
    StatValue defense{100};  // Def or SpDef depending on move
    StatStage defense_stage{DEFAULT_STAT_STAGE};
    types::enums::Type defender_type1{types::enums::Type::NONE};
    types::enums::Type defender_type2{types::enums::Type::NONE};

    // Move info
    MovePower power{40};
    types::enums::Type move_type{types::enums::Type::NORMAL};

    // Modifiers
    CritStage crit_stage{0};
    bool is_critical{false};  // Pre-rolled, or set to force crit
    bool skip_random{false};  // For deterministic testing
};

// ============================================================================
//                         DAMAGE PIPELINE HELPERS
// ============================================================================

/**
 * @brief Check if attacker gets STAB (Same Type Attack Bonus).
 */
constexpr bool has_stab(types::enums::Type move_type, types::enums::Type attacker_type1,
                        types::enums::Type attacker_type2) {
    return move_type == attacker_type1 || move_type == attacker_type2;
}

/**
 * @brief Resolve whether this hit is critical.
 */
inline bool resolve_critical_hit(const DamageParams& params) {
    if (params.is_critical) {
        return true;
    }
    if (params.crit_stage <= MAX_CRIT_STAGE) {
        return roll_critical(params.crit_stage);
    }
    return false;
}

/**
 * @brief Effective stats after applying stat stages with critical hit rules.
 */
struct EffectiveStats {
    StatValue attack;
    StatValue defense;
};

/**
 * @brief Apply stat stages, respecting critical hit rules.
 *
 * Critical hits ignore negative attack stages and positive defense stages.
 */
constexpr EffectiveStats apply_crit_aware_stat_stages(const DamageParams& params,
                                                      bool is_critical) {
    StatValue effective_attack = params.attack;
    StatValue effective_defense = params.defense;

    if (is_critical) {
        // Crit: only apply attack stage if positive
        if (params.attack_stage > DEFAULT_STAT_STAGE) {
            effective_attack = apply_stat_stage(params.attack, params.attack_stage);
        }
        // Crit: only apply defense stage if negative
        if (params.defense_stage < DEFAULT_STAT_STAGE) {
            effective_defense = apply_stat_stage(params.defense, params.defense_stage);
        }
    } else {
        effective_attack = apply_stat_stage(params.attack, params.attack_stage);
        effective_defense = apply_stat_stage(params.defense, params.defense_stage);
    }

    // Prevent division by zero
    if (effective_defense == 0)
        effective_defense = 1;

    return {effective_attack, effective_defense};
}

/**
 * @brief Calculate base damage using Gen III formula.
 *
 * Formula: ((2 * Level / 5 + 2) * Power * Attack / Defense) / 50 + 2
 */
constexpr DamageCalc calc_base_damage(Level level, MovePower power, StatValue attack,
                                      StatValue defense) {
    DamageCalc damage = 2u * level / 5u + 2u;
    damage = damage * power * attack;
    damage = damage / defense;
    damage = damage / 50u + 2u;
    return damage;
}

/**
 * @brief Apply critical hit multiplier.
 */
constexpr DamageCalc apply_critical_multiplier(DamageCalc damage, bool is_critical) {
    return is_critical ? damage * CRIT_MULTIPLIER : damage;
}

/**
 * @brief Roll and apply random variance (85-100%).
 */
inline DamageCalc apply_random_variance(DamageCalc damage, bool skip_random) {
    if (skip_random) {
        return damage;
    }
    uint16_t random_factor = 100u - util::random::Random(16);  // 85-100
    return damage * random_factor / 100u;
}

/**
 * @brief Apply STAB bonus (1.5x) if applicable.
 */
constexpr DamageCalc apply_stab(DamageCalc damage, types::enums::Type move_type,
                                types::enums::Type attacker_type1,
                                types::enums::Type attacker_type2) {
    if (has_stab(move_type, attacker_type1, attacker_type2)) {
        return damage * 3u / 2u;
    }
    return damage;
}

/**
 * @brief Apply type effectiveness multiplier.
 */
constexpr DamageCalc apply_type_effectiveness(DamageCalc damage, Effectiveness eff) {
    return damage * eff / effectiveness::DUAL_NEUTRAL;
}

/**
 * @brief Enforce minimum 1 damage for non-immune hits.
 */
constexpr DamageCalc enforce_minimum_damage(DamageCalc damage, Effectiveness eff) {
    if (damage == 0 && !is_immune(eff)) {
        return 1;
    }
    return damage;
}

/**
 * @brief Clamp damage to final Damage type range.
 */
constexpr Damage clamp_damage(DamageCalc damage) {
    return static_cast<Damage>(damage > 0xFFFF ? 0xFFFF : damage);
}

/**
 * @brief Calculate damage using Gen III formula.
 *
 * Pipeline:
 *   1. Resolve critical hit
 *   2. Apply stat stages (crit-aware)
 *   3. Calculate base damage
 *   4. Apply critical multiplier
 *   5. Apply STAB
 *   6. Apply type effectiveness
 *   7. Apply random variance (85-100%) - must be after type calc per Gen III
 *   8. Enforce minimum damage
 *
 * Reference: pokeemerald/data/battle_scripts_1.s order:
 *   critcalc -> damagecalc -> typecalc (STAB + type eff) -> adjustnormaldamage (random)
 *
 * @param params All parameters needed for damage calculation
 *
 * @return DamageResult containing final damage, effectiveness, and crit status
 */
inline DamageResult calculate_damage(const DamageParams& params) {
    DamageResult result{};

    result.critical = resolve_critical_hit(params);

    auto [atk, def] = apply_crit_aware_stat_stages(params, result.critical);

    DamageCalc damage = calc_base_damage(params.level, params.power, atk, def);
    damage = apply_critical_multiplier(damage, result.critical);
    damage = apply_stab(damage, params.move_type, params.attacker_type1, params.attacker_type2);

    result.effectiveness =
        get_type_effectiveness(params.move_type, params.defender_type1, params.defender_type2);

    damage = apply_type_effectiveness(damage, result.effectiveness);
    damage = apply_random_variance(damage, params.skip_random);
    damage = enforce_minimum_damage(damage, result.effectiveness);

    result.damage = clamp_damage(damage);
    return result;
}

}  // namespace logic::calc
