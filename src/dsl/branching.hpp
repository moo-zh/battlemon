#pragma once

/**
 * @file branching.hpp
 * @brief Conditional branching primitives for effect pipelines.
 *
 * Provides predicates, branches, and pattern matching for conditional
 * effect execution. All branch paths must converge to the same stage,
 * enforced at compile time.
 */

#include <type_traits>

#include "../logic/state/context.hpp"
#include "domain.hpp"
#include "meta.hpp"
#include "pipeline.hpp"
#include "stages.hpp"

namespace dsl {

/**
 * @brief Function pointer type for runtime predicates.
 *
 * Predicates inspect BattleContext and return bool, used for
 * conditional branching in effects.
 */
using Predicate = bool (*)(const BattleContext&);

/**
 * @brief Built-in predicates for common branching conditions.
 *
 * All predicates take a const BattleContext& and return bool.
 * Use with Branch or Match for conditional effect execution.
 */
namespace when {

/**
 * @defgroup predicates_special Special Predicates
 * @{
 */

/// Always returns true (for default branches).
inline bool Always(const BattleContext&) {
    return true;
}

/** @} */

/**
 * @defgroup predicates_weather Weather Predicates
 * @{
 */

/// True if sun is active.
inline bool InSun(const BattleContext& ctx) {
    return ctx.field && ctx.field->weather == logic::state::Weather::SUN;
}

/// True if rain is active.
inline bool InRain(const BattleContext& ctx) {
    return ctx.field && ctx.field->weather == logic::state::Weather::RAIN;
}

/// True if sandstorm is active.
inline bool InSandstorm(const BattleContext& ctx) {
    return ctx.field && ctx.field->weather == logic::state::Weather::SANDSTORM;
}

/// True if hail is active.
inline bool InHail(const BattleContext& ctx) {
    return ctx.field && ctx.field->weather == logic::state::Weather::HAIL;
}

/// True if no weather is active.
inline bool NoWeather(const BattleContext& ctx) {
    return !ctx.field || ctx.field->weather == logic::state::Weather::NONE;
}

/** @} */

/**
 * @defgroup predicates_move Move Result Predicates
 * @{
 */

/// True if the move hit.
inline bool MoveHit(const BattleContext& ctx) {
    return !ctx.result.missed;
}

/// True if the move missed.
inline bool MoveMissed(const BattleContext& ctx) {
    return ctx.result.missed;
}

/** @} */

/**
 * @defgroup predicates_target Target State Predicates
 * @{
 */

/// True if the target is alive.
inline bool TargetAlive(const BattleContext& ctx) {
    return ctx.defender_mon && ctx.defender_mon->is_alive();
}

/// True if the target has fainted.
inline bool TargetFainted(const BattleContext& ctx) {
    return ctx.defender_mon && ctx.defender_mon->is_fainted();
}

/** @} */

/**
 * @defgroup predicates_charging Charging State Predicates
 * @{
 */

/// True if the attacker is charging a two-turn move.
inline bool IsCharging(const BattleContext& ctx) {
    return ctx.attacker_slot && ctx.attacker_slot->charging_move != 0;
}

/// True if the attacker is not charging.
inline bool NotCharging(const BattleContext& ctx) {
    return !ctx.attacker_slot || ctx.attacker_slot->charging_move == 0;
}

/** @} */

}  // namespace when

/**
 * @brief Pairs a predicate with an action for conditional execution.
 * @tparam When Predicate function to evaluate.
 * @tparam Action Action to execute if predicate is true.
 *
 * The action must declare `output_stage` so Match can verify all
 * branches converge to the same stage.
 *
 * @code
 * using SunBranch = Branch<when::InSun, Do<SolarBeamFire>>;
 * @endcode
 */
template <Predicate When, typename Action>
struct Branch {
    using action_type = Action;                          ///< The wrapped action type.
    using output_stage = typename Action::output_stage;  ///< Stage after action executes.

    /// True if this is the default branch (when::Always).
    static constexpr bool is_default = (When == when::Always);

    /**
     * @brief Evaluates the predicate.
     * @param ctx Battle context to check.
     * @return True if this branch should execute.
     */
    static bool check(const BattleContext& ctx) { return When(ctx); }

    /**
     * @brief Executes the branch's action.
     * @tparam Stage Current pipeline stage.
     * @tparam Allowed Permitted domains.
     * @param pipe Current pipeline.
     * @return Pipeline at action's output stage.
     */
    template <typename Stage, Domain Allowed>
    static auto execute(Pipeline<Stage, Allowed> pipe) {
        return Action::execute(pipe);
    }
};

/// @cond INTERNAL
namespace detail {

/// Checks all branches converge to target stage.
template <typename TargetStage, typename... Branches>
struct AllConverge;

template <typename TargetStage>
struct AllConverge<TargetStage> {
    static constexpr bool value = true;
};

template <typename TargetStage, typename First, typename... Rest>
struct AllConverge<TargetStage, First, Rest...> {
    static constexpr bool value = std::is_same_v<typename First::output_stage, TargetStage> &&
                                  AllConverge<TargetStage, Rest...>::value;
};

/// Checks last branch is default.
template <typename... Branches>
struct LastIsDefault;

template <typename Last>
struct LastIsDefault<Last> {
    static constexpr bool value = Last::is_default;
};

template <typename First, typename... Rest>
struct LastIsDefault<First, Rest...> {
    static constexpr bool value = LastIsDefault<Rest...>::value;
};

/// Dispatch implementation - base case (default branch).
template <typename ConvergenceStage, typename Stage, Domain Allowed, typename First>
Pipeline<ConvergenceStage, Allowed> dispatch_impl(Pipeline<Stage, Allowed> pipe) {
    return First::execute(pipe);
}

/// Dispatch implementation - recursive case.
template <typename ConvergenceStage, typename Stage, Domain Allowed, typename First,
          typename Second, typename... Rest>
Pipeline<ConvergenceStage, Allowed> dispatch_impl(Pipeline<Stage, Allowed> pipe) {
    if (First::check(pipe.context())) {
        return First::execute(pipe);
    }
    return dispatch_impl<ConvergenceStage, Stage, Allowed, Second, Rest...>(pipe);
}

}  // namespace detail
/// @endcond

/**
 * @brief Pattern matching construct that executes the first matching branch.
 * @tparam ConvergenceStage Stage all branches must converge to.
 * @tparam Branches Branch types to evaluate in order.
 *
 * Match executes the first branch whose predicate returns true.
 * Compile-time enforcement ensures:
 * - All branches converge to ConvergenceStage
 * - Last branch is a default (when::Always)
 *
 * @code
 * using WeatherMatch = Match<EffectApplied,
 *     Branch<when::InSun, Do<SolarBeamFire>>,
 *     Branch<when::InRain, Do<SolarBeamCharge>>,
 *     Otherwise<Do<SolarBeamCharge>>
 * >;
 * @endcode
 */
template <typename ConvergenceStage, typename... Branches>
struct Match {
    static_assert(sizeof...(Branches) > 0, "Match requires at least one branch");

    static_assert(detail::AllConverge<ConvergenceStage, Branches...>::value,
                  "All branches must converge to the specified ConvergenceStage");

    static_assert(
        detail::LastIsDefault<Branches...>::value,
        "Last branch must be a default (use Otherwise<Action> or Branch<when::Always, Action>)");

    using output_stage = ConvergenceStage;  ///< Stage after match completes.

    /**
     * @brief Executes the first matching branch.
     * @tparam Stage Current pipeline stage.
     * @tparam Allowed Permitted domains.
     * @param pipe Current pipeline.
     * @return Pipeline at ConvergenceStage.
     */
    template <typename Stage, Domain Allowed>
    static Pipeline<ConvergenceStage, Allowed> execute(Pipeline<Stage, Allowed> pipe) {
        return detail::dispatch_impl<ConvergenceStage, Stage, Allowed, Branches...>(pipe);
    }
};

/**
 * @defgroup action_helpers Action Helpers
 * @brief Utilities for composing commands into actions.
 * @{
 */

/**
 * @brief Executes a sequence of commands in order.
 * @tparam Cmds Command types to execute sequentially.
 *
 * The output_stage is the last command's output stage.
 *
 * @code
 * using HitSequence = Seq<CheckAccuracy, CalculateDamage, ApplyDamage>;
 * @endcode
 */
template <typename... Cmds>
struct Seq {
    using last_cmd = meta::last_type_t<Cmds...>;           ///< Last command in sequence.
    using output_stage = typename last_cmd::output_stage;  ///< Final output stage.

    /**
     * @brief Executes all commands in sequence.
     * @tparam Stage Current pipeline stage.
     * @tparam Allowed Permitted domains.
     * @param pipe Current pipeline.
     * @return Pipeline at last command's output stage.
     */
    template <typename Stage, Domain Allowed>
    static auto execute(Pipeline<Stage, Allowed> pipe) {
        return execute_impl<Stage, Allowed, Cmds...>(pipe);
    }

   private:
    template <typename Stage, Domain Allowed, typename Cmd>
    static auto execute_impl(Pipeline<Stage, Allowed> pipe) {
        return pipe.template run<Cmd>();
    }

    template <typename Stage, Domain Allowed, typename Cmd, typename Next, typename... Rest>
    static auto execute_impl(Pipeline<Stage, Allowed> pipe) {
        auto next_pipe = pipe.template run<Cmd>();
        return execute_impl<typename Cmd::output_stage, Allowed, Next, Rest...>(next_pipe);
    }
};

/**
 * @brief Wraps a single command as an action.
 * @tparam Cmd Command type to execute.
 *
 * Useful for uniformity in Match branches.
 *
 * @code
 * using DefaultAction = Do<ApplyDamage>;
 * @endcode
 */
template <typename Cmd>
struct Do {
    using output_stage = typename Cmd::output_stage;  ///< Command's output stage.

    /**
     * @brief Executes the wrapped command.
     * @tparam Stage Current pipeline stage.
     * @tparam Allowed Permitted domains.
     * @param pipe Current pipeline.
     * @return Pipeline at command's output stage.
     */
    template <typename Stage, Domain Allowed>
    static auto execute(Pipeline<Stage, Allowed> pipe) {
        return pipe.template run<Cmd>();
    }
};

/**
 * @brief Convenience alias for a default branch.
 * @tparam Action Action to execute as fallback.
 *
 * Equivalent to `Branch<when::Always, Action>`.
 */
template <typename Action>
using Otherwise = Branch<when::Always, Action>;

/** @} */  // end of action_helpers group

}  // namespace dsl
