#pragma once

#include <cstdint>

namespace types::calc {

// ============================================================================
//                         CALC TYPE ALIASES
// ============================================================================
//
// Domain-specific type aliases for the damage/stat calculation pipeline.
// These provide self-documenting function signatures without runtime cost.
//
// Note: These are aliases, not distinct types. The compiler won't prevent
// mixing them, but they improve readability and intent.
// ============================================================================

// -----------------------------------------------------------------------------
// Damage Pipeline
// -----------------------------------------------------------------------------

/// Intermediate damage value during calculation (may exceed uint16_t range)
using DamageCalc = uint32_t;

/// Final clamped damage value (0-65535)
using Damage = uint16_t;

/// Type effectiveness multiplier (0-400, where 100 = neutral)
/// Values: 0=immune, 25=0.25x, 50=0.5x, 100=1x, 200=2x, 400=4x
using Effectiveness = uint16_t;

// -----------------------------------------------------------------------------
// Stats
// -----------------------------------------------------------------------------

/// Calculated stat value (HP, Attack, Defense, etc.)
using StatValue = uint16_t;

/// Stat stage modifier (0-12, where 6 = neutral)
using StatStage = uint8_t;

/// Nature modifier result (-1, 0, +1)
using NatureModifier = int8_t;

// -----------------------------------------------------------------------------
// Pokemon Attributes
// -----------------------------------------------------------------------------

/// Pokemon level (1-100)
using Level = uint8_t;

/// Base stat value (species-defined, 1-255)
using BaseStat = uint8_t;

/// Individual Value (0-31)
using IV = uint8_t;

/// Effort Value (0-255 per stat)
using EV = uint8_t;

/// Move base power (0-255)
using MovePower = uint16_t;

// -----------------------------------------------------------------------------
// Critical Hits
// -----------------------------------------------------------------------------

/// Critical hit stage (0-4)
using CritStage = uint8_t;

}  // namespace types::calc
