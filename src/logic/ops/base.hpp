#pragma once

#include "../../dsl/domain.hpp"
#include "../../dsl/stages.hpp"
#include "../state/context.hpp"

namespace logic::ops {

// ============================================================================
//                           COMMAND METADATA
// ============================================================================
//
// ops are the atomic operations that effects compose. Each command
// declares:
//   1. domains  - Which state domains it requires access to
//   2. input    - The pipeline stage it runs at
//   3. output   - The pipeline stage after execution
//
// The DSL validates these at compile time: an effect cannot use a command
// unless it has declared access to all required domains.
// ============================================================================

template <dsl::Domain RequiredDomains, typename InputStage, typename OutputStage>
struct CommandMeta {
    static constexpr dsl::Domain domains = RequiredDomains;
    using input_stage = InputStage;
    using output_stage = OutputStage;
};

// Shorthand for common domain combinations
using namespace dsl;
using namespace dsl::domains;

}  // namespace logic::ops
