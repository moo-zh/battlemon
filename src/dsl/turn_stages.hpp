#pragma once

/**
 * @file turn_stages.hpp
 * @brief Turn-level pipeline stages for engine orchestration.
 *
 * The turn pipeline wraps the move pipeline, providing structured hook points
 * for item/ability effects that fire at turn boundaries rather than during
 * move execution.
 *
 * Orchestration model: Turn stages provide hook points, but complex sequencing
 * (speed ties, forced switches, faint handling) remains imperative in the engine.
 * The pipeline gives the *where*, engine handles the *how*.
 */

#include <cstdint>
#include <type_traits>

namespace dsl::turn {

// ============================================================================
//                          TURN PIPELINE STAGES
// ============================================================================
//
// Turn progression:
//   TurnGenesis -> PriorityDetermined -> ActionsResolving ->
//   ActionsResolved -> TurnEnd -> TurnTerminus
//
// ============================================================================

/// Initial stage - turn begins
struct TurnGenesis {};

/// After priority/speed determination (Quick Claw fires here)
struct PriorityDetermined {};

/// While moves are executing (wraps move pipeline per-action)
struct ActionsResolving {};

/// After all moves complete
struct ActionsResolved {};

/// End-of-turn effects (Leftovers, weather damage, etc.)
struct TurnEnd {};

/// Turn complete
struct TurnTerminus {};

// ============================================================================
//                          STAGE RANKING
// ============================================================================

enum class TurnStageRank : uint8_t {
    TurnGenesis = 0,
    PriorityDetermined,
    ActionsResolving,
    ActionsResolved,
    TurnEnd,
    TurnTerminus,
};

template <typename S>
struct turn_stage_rank;

template <>
struct turn_stage_rank<TurnGenesis>
    : std::integral_constant<uint8_t, static_cast<uint8_t>(TurnStageRank::TurnGenesis)> {};
template <>
struct turn_stage_rank<PriorityDetermined>
    : std::integral_constant<uint8_t, static_cast<uint8_t>(TurnStageRank::PriorityDetermined)> {};
template <>
struct turn_stage_rank<ActionsResolving>
    : std::integral_constant<uint8_t, static_cast<uint8_t>(TurnStageRank::ActionsResolving)> {};
template <>
struct turn_stage_rank<ActionsResolved>
    : std::integral_constant<uint8_t, static_cast<uint8_t>(TurnStageRank::ActionsResolved)> {};
template <>
struct turn_stage_rank<TurnEnd>
    : std::integral_constant<uint8_t, static_cast<uint8_t>(TurnStageRank::TurnEnd)> {};
template <>
struct turn_stage_rank<TurnTerminus>
    : std::integral_constant<uint8_t, static_cast<uint8_t>(TurnStageRank::TurnTerminus)> {};

template <typename A, typename B>
struct TurnStageLeq : std::bool_constant<(turn_stage_rank<A>::value <= turn_stage_rank<B>::value)> {
};

template <typename A, typename B>
inline constexpr bool turn_stage_leq_v = TurnStageLeq<A, B>::value;

// ============================================================================
//                          STAGE NAMES
// ============================================================================

namespace stages {

template <typename S>
constexpr const char* name = "Unknown";

template <>
inline constexpr const char* name<TurnGenesis> = "TurnGenesis";
template <>
inline constexpr const char* name<PriorityDetermined> = "PriorityDetermined";
template <>
inline constexpr const char* name<ActionsResolving> = "ActionsResolving";
template <>
inline constexpr const char* name<ActionsResolved> = "ActionsResolved";
template <>
inline constexpr const char* name<TurnEnd> = "TurnEnd";
template <>
inline constexpr const char* name<TurnTerminus> = "TurnTerminus";

}  // namespace stages

}  // namespace dsl::turn
