#pragma once

/**
 * @file stages.hpp
 * @brief Pipeline stage types for compile-time effect ordering validation.
 *
 * Effects execute as a pipeline through well-defined stages. Each command
 * declares its input stage (when it can run) and output stage (where the
 * pipeline advances to after execution).
 *
 * Stage progression is enforced at compile time. The pipeline tracks the
 * current stage, and commands can only run if the pipeline has reached
 * (or passed) their required input stage.
 */

#include <cstdint>
#include <type_traits>

namespace dsl {

/**
 * @defgroup stages Pipeline Stages
 * @brief Tag types representing ordered stages in effect execution.
 *
 * Progression:
 * @code
 * Genesis -> AccuracyResolved -> DamageCalculated ->
 * DamageApplied -> EffectApplied -> FaintChecked -> Terminus
 * @endcode
 * @{
 */

/// Initial stage before any effect logic executes.
struct Genesis {};

/// After accuracy check has been performed.
struct AccuracyResolved {};

/// After damage value has been calculated.
struct DamageCalculated {};

/// After damage has been applied to target.
struct DamageApplied {};

/// After secondary effects have been applied.
struct EffectApplied {};

/// After faint checks have been performed.
struct FaintChecked {};

/// Final stage indicating effect completion.
struct Terminus {};

/** @} */  // end of stages group

/**
 * @brief Numeric ranking for stage ordering comparisons.
 *
 * Used internally by StageLeq to compare stages without requiring
 * explicit specializations for every pair.
 */
enum class StageRank : uint8_t {
    Genesis = 0,
    AccuracyResolved,
    DamageCalculated,
    DamageApplied,
    EffectApplied,
    FaintChecked,
    Terminus,
};

/**
 * @brief Maps a stage type to its numeric rank.
 * @tparam S Stage type (Genesis, AccuracyResolved, etc.)
 */
template <typename S>
struct stage_rank;

/// @cond INTERNAL
template <>
struct stage_rank<Genesis>
    : std::integral_constant<uint8_t, static_cast<uint8_t>(StageRank::Genesis)> {};
template <>
struct stage_rank<AccuracyResolved>
    : std::integral_constant<uint8_t, static_cast<uint8_t>(StageRank::AccuracyResolved)> {};
template <>
struct stage_rank<DamageCalculated>
    : std::integral_constant<uint8_t, static_cast<uint8_t>(StageRank::DamageCalculated)> {};
template <>
struct stage_rank<DamageApplied>
    : std::integral_constant<uint8_t, static_cast<uint8_t>(StageRank::DamageApplied)> {};
template <>
struct stage_rank<EffectApplied>
    : std::integral_constant<uint8_t, static_cast<uint8_t>(StageRank::EffectApplied)> {};
template <>
struct stage_rank<FaintChecked>
    : std::integral_constant<uint8_t, static_cast<uint8_t>(StageRank::FaintChecked)> {};
template <>
struct stage_rank<Terminus>
    : std::integral_constant<uint8_t, static_cast<uint8_t>(StageRank::Terminus)> {};
/// @endcond

/**
 * @brief Compile-time check if stage A precedes or equals stage B.
 * @tparam A First stage type.
 * @tparam B Second stage type.
 *
 * Inherits from std::true_type if A <= B in the stage ordering,
 * std::false_type otherwise.
 */
template <typename A, typename B>
struct StageLeq : std::bool_constant<(stage_rank<A>::value <= stage_rank<B>::value)> {};

/**
 * @brief Helper variable template for StageLeq.
 * @tparam A First stage type.
 * @tparam B Second stage type.
 */
template <typename A, typename B>
inline constexpr bool stage_leq_v = StageLeq<A, B>::value;

/**
 * @brief Compile-time stage name strings for debugging/logging.
 */
namespace stages {

/// Returns the human-readable name of a stage type.
template <typename S>
constexpr const char* name = "Unknown";

/// @cond INTERNAL
template <>
inline constexpr const char* name<Genesis> = "Genesis";
template <>
inline constexpr const char* name<AccuracyResolved> = "AccuracyResolved";
template <>
inline constexpr const char* name<DamageCalculated> = "DamageCalculated";
template <>
inline constexpr const char* name<DamageApplied> = "DamageApplied";
template <>
inline constexpr const char* name<EffectApplied> = "EffectApplied";
template <>
inline constexpr const char* name<FaintChecked> = "FaintChecked";
template <>
inline constexpr const char* name<Terminus> = "Terminus";
/// @endcond

}  // namespace stages

}  // namespace dsl
