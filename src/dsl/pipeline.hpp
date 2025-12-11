#pragma once

/**
 * @file pipeline.hpp
 * @brief Type-state pipeline for compile-time validated effect execution.
 *
 * The Pipeline class tracks stage and domain information at compile time,
 * ensuring effects only access permitted state and follow proper stage
 * ordering. Invalid usage results in compile errors rather than runtime bugs.
 */

#include "domain.hpp"
#include "meta.hpp"
#include "stages.hpp"
#include "transition.hpp"

namespace dsl {

// Forward declaration
struct BattleContext;

/**
 * @brief Type-state pipeline tracking stage and domain access at compile time.
 * @tparam Stage Current stage in effect execution.
 * @tparam Allowed Domains this effect may access.
 *
 * The Pipeline tracks two pieces of compile-time information:
 *   1. **Stage** - Where we are in the effect execution
 *   2. **Allowed** - Which domains this effect may access
 *
 * The `run<Cmd>()` method advances the pipeline:
 *   - Validates domain access at compile time
 *   - Validates stage progression at compile time
 *   - Executes the command at runtime
 *   - Returns a new Pipeline at the command's output stage
 *
 * If validation fails, you get a clear compile error pointing to the
 * exact constraint that was violated.
 *
 * @see begin() Factory function to create pipelines.
 */
template <typename Stage, Domain Allowed>
class Pipeline {
    BattleContext* ctx_;

   public:
    using stage_type = Stage;                           ///< Current pipeline stage.
    static constexpr Domain allowed_domains = Allowed;  ///< Permitted domain access.

    /**
     * @brief Constructs a pipeline with the given battle context.
     * @param ctx Battle context to operate on.
     */
    explicit constexpr Pipeline(BattleContext& ctx) : ctx_(&ctx) {}

    /**
     * @brief Executes a Command, validating domain and stage requirements.
     * @tparam Cmd Command type to execute.
     * @return New pipeline at Cmd's output stage.
     *
     * Compile-time constraints:
     * - Cmd::domains must be a subset of Allowed
     * - Stage must have reached Cmd::input_stage
     */
    template <meta::Command Cmd>
        requires meta::ValidAccess<Allowed, Cmd::domains> &&
                 meta::StageReached<Stage, typename Cmd::input_stage>
    [[nodiscard]] constexpr auto run() -> Pipeline<typename Cmd::output_stage, Allowed> {
        if constexpr (meta::TransientCommand<Cmd>) {
            auto payload = Cmd::build_transient(*ctx_);
            StageTransition<Stage, typename Cmd::output_stage>::execute(*ctx_, payload);
            Cmd::execute(*ctx_, payload);
            return Pipeline<typename Cmd::output_stage, Allowed>{*ctx_};
        } else {
            static_assert(!meta::RequestsTransient<Cmd> || meta::TransientCommand<Cmd>,
                          "Command declares Domain::Transient but is missing transient "
                          "payload support");

            StageTransition<Stage, typename Cmd::output_stage>::execute(*ctx_);
            Cmd::execute(*ctx_);
            return Pipeline<typename Cmd::output_stage, Allowed>{*ctx_};
        }
    }

    /**
     * @brief Executes a Command with arguments.
     * @tparam Cmd Command type to execute.
     * @tparam Args Argument types.
     * @param args Arguments to pass to Cmd::execute.
     * @return New pipeline at Cmd's output stage.
     */
    template <meta::Command Cmd, typename... Args>
        requires meta::ValidAccess<Allowed, Cmd::domains> &&
                 meta::StageReached<Stage, typename Cmd::input_stage>
    [[nodiscard]] constexpr auto run(Args&&... args)
        -> Pipeline<typename Cmd::output_stage, Allowed> {
        if constexpr (meta::TransientCommand<Cmd>) {
            auto payload = Cmd::build_transient(*ctx_);
            StageTransition<Stage, typename Cmd::output_stage>::execute(*ctx_, payload);
            Cmd::execute(*ctx_, payload, static_cast<Args&&>(args)...);
            return Pipeline<typename Cmd::output_stage, Allowed>{*ctx_};
        } else {
            static_assert(!meta::RequestsTransient<Cmd> || meta::TransientCommand<Cmd>,
                          "Command declares Domain::Transient but is missing transient "
                          "payload support");

            StageTransition<Stage, typename Cmd::output_stage>::execute(*ctx_);
            Cmd::execute(*ctx_, static_cast<Args&&>(args)...);
            return Pipeline<typename Cmd::output_stage, Allowed>{*ctx_};
        }
    }

    /**
     * @brief Executes an Action (Match, Seq, etc.).
     * @tparam Act Action type to execute.
     * @return New pipeline at Act's output stage.
     *
     * Actions are composite operations that delegate back to the pipeline.
     */
    template <meta::Action Act>
    [[nodiscard]] constexpr auto run() -> Pipeline<typename Act::output_stage, Allowed> {
        return Act::execute(*this);
    }

    /**
     * @brief Advances to a later stage without executing a command.
     * @tparam TargetStage Stage to advance to.
     * @return New pipeline at TargetStage.
     *
     * Used for convergence after branching, ensuring all paths reach
     * the same stage.
     */
    template <typename TargetStage>
        requires meta::StageReached<TargetStage, Stage>
    [[nodiscard]] constexpr auto advance_to() -> Pipeline<TargetStage, Allowed> {
        return Pipeline<TargetStage, Allowed>{*ctx_};
    }

    /**
     * @brief Accesses the battle context.
     * @return Reference to the battle context.
     *
     * Used by predicates in branching logic.
     */
    [[nodiscard]] constexpr BattleContext& context() const { return *ctx_; }

    /**
     * @brief Validates that the pipeline can reach Terminus.
     *
     * Static assertion ensures the current stage allows progression
     * to the terminal stage.
     */
    constexpr void end() const {
        static_assert(stage_leq_v<Stage, Terminus>, "Pipeline must be able to reach Terminus");
    }
};

/**
 * @brief Creates a new pipeline at Genesis stage with specified domain access.
 * @tparam Allowed Domains the effect may access.
 * @param ctx Battle context to operate on.
 * @return Pipeline starting at Genesis with the specified domain permissions.
 *
 * @code
 * auto pipe = begin<domains::Pure>(ctx);
 * @endcode
 */
template <Domain Allowed>
[[nodiscard]] constexpr Pipeline<Genesis, Allowed> begin(BattleContext& ctx) {
    return Pipeline<Genesis, Allowed>{ctx};
}

}  // namespace dsl
