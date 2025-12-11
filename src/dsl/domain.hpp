#pragma once

/**
 * @file domain.hpp
 * @brief Domain system for compile-time access control in battle effects.
 *
 * Defines the four domains representing different scopes of battle state,
 * each with distinct lifecycle rules. Derived from pokeemerald's
 * SwitchInClearSetData().
 */

#include <cstdint>

namespace dsl {

/**
 * @brief Represents distinct scopes of battle state with different lifecycles.
 *
 * The domain system enables compile-time validation that effects only access
 * state they're permitted to modify. Commands declare required domains, and
 * effects declare allowed domains - mismatches cause compile errors.
 *
 * | Domain    | Scope           | Lifecycle                          |
 * |-----------|-----------------|-----------------------------------|
 * | Field     | Global          | Persists entire battle            |
 * | Side      | Per-team        | Persists entire battle            |
 * | Slot      | Per-position    | Cleared on switch                 |
 * | Mon       | Per-pokemon     | Persists through switches         |
 * | Transient | Scoped scratch  | Exists only for the current op    |
 */
enum class Domain : uint8_t {
    Field = 1 << 0,      ///< Global state (weather, future sight, wish).
    Side = 1 << 1,       ///< Per-team state (screens, hazards).
    Slot = 1 << 2,       ///< Per-position state (stat stages, volatiles).
    Mon = 1 << 3,        ///< Per-pokemon state (HP, status, PP).
    Transient = 1 << 4,  ///< Scoped scratch data with no persistence.
};

/**
 * @brief Combines two domains using bitwise OR.
 * @param a First domain.
 * @param b Second domain.
 * @return Combined domain containing both scopes.
 */
constexpr Domain operator|(Domain a, Domain b) {
    return static_cast<Domain>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

/**
 * @brief Checks if two domains overlap using bitwise AND.
 * @param a First domain.
 * @param b Second domain.
 * @return True if any domain bits are shared.
 */
constexpr bool operator&(Domain a, Domain b) {
    return (static_cast<uint8_t>(a) & static_cast<uint8_t>(b)) != 0;
}

/**
 * @brief Pre-defined domain combinations for common effect patterns.
 *
 * These aliases reduce boilerplate and improve readability when declaring
 * effect domain requirements.
 */
namespace domains {

/// Pure damage/status effects accessing slot and mon state (most common).
constexpr Domain Pure = Domain::Slot | Domain::Mon;

/// Weather-dependent effects (Solar Beam, Weather Ball).
constexpr Domain Weather = Domain::Field | Domain::Slot | Domain::Mon;

/// Screen effects (Reflect, Light Screen).
constexpr Domain Screen = Domain::Side;

/// Hazard effects (Spikes, Rapid Spin).
constexpr Domain Hazards = Domain::Side;

/// Pure status application (only mon state).
constexpr Domain Status = Domain::Mon;

/// Pure stat modification (only slot state).
constexpr Domain StatChange = Domain::Slot;

/// Full access to all domains (for complex effects like Baton Pass).
constexpr Domain All = Domain::Field | Domain::Side | Domain::Slot | Domain::Mon;

/// Scoped scratch access (must be explicitly opted into).
constexpr Domain Scratch = Domain::Transient;

}  // namespace domains

}  // namespace dsl
