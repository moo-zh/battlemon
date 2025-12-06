#pragma once

/**
 * @file effect.hpp
 * @brief Effect DSL for zero-cost, compile-time validated battle effects.
 *
 * Provides both declarative and imperative styles for defining effects:
 *
 * **Declarative (type aliases):**
 * @code
 * using Hit = Effect<Pure,
 *     Seq<CheckAccuracy, CalculateDamage, ApplyDamage, CheckFaint>
 * >;
 * @endcode
 *
 * **Imperative (macro-based):**
 * @code
 * EFFECT(Hit, Pure) {
 *     BEGIN(ctx);
 *     RUN(CheckAccuracy);
 *     RUN(CalculateDamage);
 *     RUN(ApplyDamage);
 *     RUN(CheckFaint);
 *     END;
 * }
 * @endcode
 *
 * Both provide full compile-time validation of:
 * - Domain access (Field, Side, Slot, Mon)
 * - Stage ordering (Genesis -> Accuracy -> Damage -> ... -> Terminus)
 */

#include <cstddef>
#include <cstdint>

#include "../logic/ops/all.hpp"
#include "../logic/state/context.hpp"
#include "branching.hpp"
#include "domain.hpp"
#include "meta.hpp"
#include "pipeline.hpp"
#include "stages.hpp"

namespace dsl {

/**
 * @brief Declarative effect wrapper with domain constraints.
 * @tparam AllowedDomains Domains this effect may access.
 * @tparam Action Action type defining the effect's behavior.
 *
 * Wraps an Action with domain constraints. The Action's output_stage
 * becomes the Effect's output_stage.
 *
 * @code
 * using Hit = Effect<Pure, Seq<CheckAccuracy, CalculateDamage, ApplyDamage>>;
 * @endcode
 */
template <Domain AllowedDomains, typename Action>
struct Effect {
    static constexpr Domain domains = AllowedDomains;    ///< Permitted domains.
    using output_stage = typename Action::output_stage;  ///< Final stage.

    /**
     * @brief Executes the effect on the given battle context.
     * @param ctx Battle context to operate on.
     */
    static void execute(BattleContext& ctx) {
        auto pipe = begin<AllowedDomains>(ctx);
        Action::execute(pipe);
    }
};

/**
 * @defgroup repetition Repetition Actions
 * @brief Actions for repeating commands or sequences.
 * @{
 */

/**
 * @brief Executes an action exactly N times.
 * @tparam N Number of repetitions.
 * @tparam Action Action to repeat.
 *
 * Converges to Action::output_stage.
 *
 * @code
 * using DoubleKick = Repeat<2, Do<FuryCutterHit>>;
 * @endcode
 */
template <std::size_t N, typename Action>
struct Repeat {
    using output_stage = typename Action::output_stage;  ///< Final stage.

    /**
     * @brief Executes the action N times.
     * @tparam Stage Current pipeline stage.
     * @tparam Allowed Permitted domains.
     * @param pipe Current pipeline.
     * @return Pipeline at Action's output stage.
     */
    template <typename Stage, Domain Allowed>
    static auto execute(Pipeline<Stage, Allowed> pipe) {
        return unroll<Stage, Allowed, 0>(pipe);
    }

   private:
    template <typename Stage, Domain Allowed, std::size_t I>
    static auto unroll(Pipeline<Stage, Allowed> pipe) {
        if constexpr (I == N) {
            return pipe.template advance_to<output_stage>();
        } else {
            auto next = Action::execute(pipe);
            return unroll<typename Action::output_stage, Allowed, I + 1>(next);
        }
    }
};

/**
 * @brief Executes an action up to N times while a predicate holds.
 * @tparam MaxIterations Maximum number of repetitions.
 * @tparam Pred Predicate to check before each iteration.
 * @tparam Action Action to repeat.
 * @tparam Convergence Stage to converge to (defaults to Action::output_stage).
 *
 * Early exit if predicate fails. The loop iteration count is stored
 * in ctx.loop_iteration for effects like Triple Kick.
 *
 * @code
 * using TripleKick = RepeatWhile<3, when::MoveHit, HitAction, FaintChecked>;
 * @endcode
 */
template <std::size_t MaxIterations, Predicate Pred, typename Action,
          typename Convergence = typename Action::output_stage>
struct RepeatWhile {
    using output_stage = Convergence;  ///< Convergence stage.

    /**
     * @brief Executes the action up to MaxIterations times.
     * @tparam Stage Current pipeline stage.
     * @tparam Allowed Permitted domains.
     * @param pipe Current pipeline.
     * @return Pipeline at Convergence stage.
     */
    template <typename Stage, Domain Allowed>
    static auto execute(Pipeline<Stage, Allowed> pipe) {
        return unroll<Stage, Allowed, 0>(pipe);
    }

   private:
    template <typename Stage, Domain Allowed, std::size_t I>
    static auto unroll(Pipeline<Stage, Allowed> pipe) {
        if constexpr (I == MaxIterations) {
            return pipe.template advance_to<Convergence>();
        } else {
            pipe.context().loop_iteration = static_cast<uint8_t>(I);

            if (!Pred(pipe.context())) {
                return pipe.template advance_to<Convergence>();
            }

            auto next = Action::execute(pipe);
            return unroll<typename Action::output_stage, Allowed, I + 1>(next);
        }
    }
};

/** @} */  // end of repetition group

}  // namespace dsl

/**
 * @defgroup imperative_macros Imperative Effect Macros
 * @brief Macro-based syntax for defining effects with compile-time safety.
 *
 * These macros provide imperative syntax while maintaining full compile-time
 * validation. Each RUN shadows the pipeline variable with a new type,
 * enabling stage tracking through scopes.
 *
 * @warning Do not use `else` after these macros.
 * @{
 */

/**
 * @def EFFECT(name, required_domains)
 * @brief Defines an effect struct with domain constraints.
 * @param name Effect name (creates Effect_##name struct and name alias).
 * @param required_domains Domain constraints for the effect.
 *
 * @code
 * EFFECT(Tackle, Pure) {
 *     BEGIN(ctx);
 *     RUN(CheckAccuracy);
 *     RUN(CalculateDamage);
 *     RUN(ApplyDamage);
 *     END;
 * }
 * @endcode
 */
#define EFFECT(name, required_domains)                             \
    struct Effect_##name {                                         \
        static constexpr ::dsl::Domain domains = required_domains; \
        static void execute(::dsl::BattleContext& ctx);            \
    };                                                             \
    using name = Effect_##name;                                    \
    inline void Effect_##name::execute(::dsl::BattleContext& ctx)

/**
 * @def BEGIN(ctx)
 * @brief Starts the pipeline in an effect body.
 * @param ctx BattleContext reference.
 */
#define BEGIN(ctx) auto _dsl_pipe = [&]() {                                                   \
        if (auto _dsl_prev = ::dsl::begin<domains>(ctx); true)
/**
 * @def RUN(cmd)
 * @brief Executes a command (auto-prefixed with logic::ops::).
 * @param cmd Command name without namespace prefix.
 */
#define RUN(cmd)                                                        \
    if (auto _pipe = _dsl_prev.template run<::logic::ops::cmd>(); true) \
        if (auto _dsl_prev = _pipe; true)

/**
 * @def RUN_WITH(cmd, ...)
 * @brief Executes a command with arguments.
 * @param cmd Command name without namespace prefix.
 * @param ... Arguments to pass to the command.
 */
#define RUN_WITH(cmd, ...)                                                         \
    if (auto _pipe = _dsl_prev.template run<::logic::ops::cmd>(__VA_ARGS__); true) \
        if (auto _dsl_prev = _pipe; true)

/**
 * @def RUN_ACTION(action)
 * @brief Executes an action (Seq, Match, Repeat, etc.).
 * @param action Action type to execute.
 */
#define RUN_ACTION(action)                                   \
    if (auto _pipe = _dsl_prev.template run<action>(); true) \
        if (auto _dsl_prev = _pipe; true)

/**
 * @def END
 * @brief Ends the pipeline and closes all scopes.
 */
#define END           \
    return _dsl_prev; \
    }                 \
    ();               \
    (void)_dsl_pipe

/** @} */  // end of imperative_macros group

/**
 * @defgroup branching_macros Branching Macros
 * @brief Convenience macros for conditional branching in effects.
 * @{
 */

/**
 * @def WHEN(predicate, action)
 * @brief Creates a Branch with a predicate from the when namespace.
 * @param predicate Predicate name (from dsl::when).
 * @param action Action to execute if predicate is true.
 *
 * @code
 * RUN_ACTION(Match<FaintChecked,
 *     WHEN(InSun, Do<SolarBeamFire>),
 *     OTHERWISE(Do<BeginCharge>)
 * >);
 * @endcode
 */
#define WHEN(predicate, action) ::dsl::Branch<::dsl::when::predicate, action>

/**
 * @def OTHERWISE(action)
 * @brief Creates a default branch (always matches).
 * @param action Action to execute as fallback.
 */
#define OTHERWISE(action) ::dsl::Branch<::dsl::when::Always, action>

/**
 * @def MATCH(convergence, ...)
 * @brief Creates a Match with the specified convergence stage.
 * @param convergence Stage all branches must converge to.
 * @param ... Branch types to evaluate.
 */
#define MATCH(convergence, ...) ::dsl::Match<convergence, __VA_ARGS__>

/** @} */  // end of branching_macros group
