#pragma once

/**
 * @file meta.hpp
 * @brief Metaprogramming utilities for compile-time DSL validation.
 *
 * Provides concepts and type traits that validate domain access and stage
 * transitions at compile time. Invalid code fails to compile with clear
 * error messages.
 */

#include <concepts>
#include <type_traits>

#include "domain.hpp"
#include "stages.hpp"

namespace dsl {

// Forward declaration
struct BattleContext;

/**
 * @brief Metaprogramming utilities for compile-time validation.
 */
namespace meta {

/**
 * @defgroup domain_validation Domain Validation
 * @brief Compile-time checks for domain access permissions.
 * @{
 */

/**
 * @brief Checks if Required domains are a subset of Allowed domains.
 * @tparam Allowed The domains permitted by the effect.
 * @tparam Required The domains needed by the command.
 *
 * True if every bit set in Required is also set in Allowed.
 */
template <Domain Allowed, Domain Required>
inline constexpr bool ValidAccess =
    (static_cast<uint8_t>(Required) & ~static_cast<uint8_t>(Allowed)) == 0;

/**
 * @brief Checks if a Command requests Transient (scratch) access.
 * @tparam Cmd Command type.
 */
template <typename Cmd>
inline constexpr bool RequestsTransient =
    (static_cast<uint8_t>(Cmd::domains) & static_cast<uint8_t>(Domain::Transient)) != 0;

/** @} */  // end of domain_validation group

/**
 * @defgroup stage_validation Stage Validation
 * @brief Compile-time checks for pipeline stage progression.
 * @{
 */

/**
 * @brief Concept satisfied when Current stage has reached Required stage.
 * @tparam Current The pipeline's current stage.
 * @tparam Required The stage needed by the command.
 *
 * A command requiring AccuracyResolved can run at AccuracyResolved or any
 * later stage (DamageCalculated, DamageApplied, etc.).
 */
template <typename Current, typename Required>
concept StageReached = stage_leq_v<Required, Current>;

/** @} */  // end of stage_validation group

/**
 * @defgroup type_detection Type Detection
 * @brief Concepts for identifying Commands and Actions.
 * @{
 */

/**
 * @brief Concept for Command types.
 *
 * A Command is a type with:
 * - `input_stage` type alias (required pipeline stage)
 * - `output_stage` type alias (resulting pipeline stage)
 * - `domains` static constant (required domain access)
 * - `execute()` static method
 */
template <typename T>
concept Command = requires {
    typename T::input_stage;
    typename T::output_stage;
    { T::domains } -> std::same_as<const Domain&>;
};

/**
 * @brief Command that declares transient scratch payload.
 *
 * Requires Domain::Transient in domains mask, transient_type alias,
 * build_transient(ctx) factory, and execute(ctx, transient_type&) overload.
 */
template <typename T>
concept TransientCommand = Command<T> && RequestsTransient<T> &&
                           requires(BattleContext& ctx, typename T::transient_type& payload) {
                               typename T::transient_type;
                               {
                                   T::build_transient(ctx)
                               } -> std::convertible_to<typename T::transient_type>;
                               { T::execute(ctx, payload) };
                           };

/**
 * @brief Concept for Action types (Seq, Match, etc.).
 *
 * An Action is a type with `output_stage` but not a full Command.
 * Actions compose Commands and delegate to the pipeline.
 */
template <typename T>
concept Action = requires { typename T::output_stage; } && !Command<T>;

/** @} */  // end of type_detection group

/**
 * @defgroup type_traits Type Traits
 * @brief Compile-time type manipulation utilities.
 * @{
 */

/**
 * @brief Extracts the last type from a parameter pack.
 * @tparam First First type in the pack.
 * @tparam Rest Remaining types.
 */
template <typename First, typename... Rest>
struct LastType {
    using type = typename LastType<Rest...>::type;
};

/// @cond INTERNAL
template <typename Last>
struct LastType<Last> {
    using type = Last;
};
/// @endcond

/**
 * @brief Helper alias for LastType.
 * @tparam Ts Types to extract last from.
 */
template <typename... Ts>
using last_type_t = typename LastType<Ts...>::type;

/** @} */  // end of type_traits group

}  // namespace meta

}  // namespace dsl
