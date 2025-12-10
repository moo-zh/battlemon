#pragma once

// ============================================================================
//                          ALL ops
// ============================================================================
//
// Convenience header that includes all command definitions.
// ============================================================================

#include "accuracy.hpp"
#include "base.hpp"
#include "damage.hpp"
#include "faint.hpp"
#include "field.hpp"
#include "stats.hpp"
#include "status.hpp"
#include "switch.hpp"

namespace logic::ops {

// Re-export common stage types for convenience
using dsl::AccuracyResolved;
using dsl::DamageApplied;
using dsl::DamageCalculated;
using dsl::EffectApplied;
using dsl::FaintChecked;
using dsl::Genesis;
using dsl::Terminus;

}  // namespace logic::ops
