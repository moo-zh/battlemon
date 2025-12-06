#pragma once

/**
 * @file runtime.hpp
 * @brief Runtime pipeline alternative for dynamic control flow.
 *
 * Provides a pipeline that tracks stage at runtime (StageId) while keeping
 * domain checks at compile time. Useful when mutation-like flows (dynamic
 * loops/branches) would make the type-state path too awkward.
 *
 * This is opt-in; the compile-time Pipeline in pipeline.hpp remains the
 * zero-cost default.
 */

#include <cassert>
#include <cstdint>

#include "../logic/state/context.hpp"
#include "domain.hpp"
#include "meta.hpp"
#include "stages.hpp"

namespace dsl::rt {

/**
 * @brief Runtime stage identifier for dynamic stage tracking.
 *
 * Maps to the compile-time stage types, allowing runtime stage checks
 * with assertion-based validation.
 */
enum class StageId : uint8_t {
    Genesis = 0,
    AccuracyResolved,
    DamageCalculated,
    DamageApplied,
    EffectApplied,
    FaintChecked,
    Terminus,
};

/**
 * @brief Maps compile-time stage types to runtime StageId values.
 * @tparam Stage Compile-time stage type.
 */
template <typename Stage>
struct stage_id;

/// @cond INTERNAL
template <>
struct stage_id<Genesis> {
    static constexpr StageId value = StageId::Genesis;
};
template <>
struct stage_id<AccuracyResolved> {
    static constexpr StageId value = StageId::AccuracyResolved;
};
template <>
struct stage_id<DamageCalculated> {
    static constexpr StageId value = StageId::DamageCalculated;
};
template <>
struct stage_id<DamageApplied> {
    static constexpr StageId value = StageId::DamageApplied;
};
template <>
struct stage_id<EffectApplied> {
    static constexpr StageId value = StageId::EffectApplied;
};
template <>
struct stage_id<FaintChecked> {
    static constexpr StageId value = StageId::FaintChecked;
};
template <>
struct stage_id<Terminus> {
    static constexpr StageId value = StageId::Terminus;
};
/// @endcond

/**
 * @brief Compares two StageId values for ordering.
 * @param a First stage.
 * @param b Second stage.
 * @return True if a precedes or equals b.
 */
inline constexpr bool stage_leq(StageId a, StageId b) {
    return static_cast<uint8_t>(a) <= static_cast<uint8_t>(b);
}

/**
 * @brief Runtime pipeline with dynamic stage tracking.
 * @tparam Allowed Domains this effect may access (compile-time checked).
 *
 * Unlike the compile-time Pipeline, this version tracks stage at runtime
 * using StageId. Domain access is still validated at compile time, but
 * stage progression is checked via assertions.
 *
 * Returns `*this` from run() to enable method chaining:
 * @code
 * rt::begin<domains::Pure>(ctx)
 *     .run<CheckAccuracy>()
 *     .run<CalculateDamage>()
 *     .run<ApplyDamage>();
 * @endcode
 */
template <Domain Allowed>
class Pipeline {
    BattleContext* ctx_;
    StageId stage_;

   public:
    /**
     * @brief Constructs a runtime pipeline at Genesis stage.
     * @param ctx Battle context to operate on.
     */
    explicit Pipeline(BattleContext& ctx) : ctx_(&ctx), stage_(StageId::Genesis) {}

    /// Returns mutable reference to the battle context.
    BattleContext& context() { return *ctx_; }

    /// Returns const reference to the battle context.
    const BattleContext& context() const { return *ctx_; }

    /**
     * @brief Executes a command with runtime stage validation.
     * @tparam Cmd Command type to execute.
     * @return Reference to this pipeline for chaining.
     *
     * Asserts that the current stage has reached Cmd's input_stage.
     */
    template <meta::Command Cmd>
    requires meta::ValidAccess<Allowed, Cmd::domains> Pipeline& run() {
        constexpr StageId required = stage_id<typename Cmd::input_stage>::value;
        constexpr StageId output = stage_id<typename Cmd::output_stage>::value;
        assert(stage_leq(required, stage_) && "Stage precondition violated (runtime)");
        Cmd::execute(*ctx_);
        stage_ = output;
        return *this;
    }

    /**
     * @brief Executes a command with arguments and runtime stage validation.
     * @tparam Cmd Command type to execute.
     * @tparam Args Argument types.
     * @param args Arguments to pass to Cmd::execute.
     * @return Reference to this pipeline for chaining.
     */
    template <meta::Command Cmd, typename... Args>
    requires meta::ValidAccess<Allowed, Cmd::domains> Pipeline& run(Args&&... args) {
        constexpr StageId required = stage_id<typename Cmd::input_stage>::value;
        constexpr StageId output = stage_id<typename Cmd::output_stage>::value;
        assert(stage_leq(required, stage_) && "Stage precondition violated (runtime)");
        Cmd::execute(*ctx_, static_cast<Args&&>(args)...);
        stage_ = output;
        return *this;
    }

    /// Returns the current runtime stage.
    StageId stage() const { return stage_; }
};

/**
 * @brief Creates a runtime pipeline at Genesis stage.
 * @tparam Allowed Domains the effect may access.
 * @param ctx Battle context to operate on.
 * @return Runtime pipeline with the specified domain permissions.
 */
template <Domain Allowed>
inline Pipeline<Allowed> begin(BattleContext& ctx) {
    return Pipeline<Allowed>(ctx);
}

}  // namespace dsl::rt
