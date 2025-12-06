#pragma once

#include "base.hpp"

namespace logic::ops {

// ============================================================================
//                              WEATHER
// ============================================================================
//
// ops for setting weather conditions.
//
// Domain: Field
// Stage:  Genesis -> EffectApplied
// ============================================================================

template <logic::state::Weather W>
struct SetWeather : CommandMeta<Domain::Field, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        // Check if weather is already active
        if (ctx.field->weather == W) {
            ctx.result.failed = true;
            return;
        }

        ctx.field->weather = W;
        ctx.field->weather_turns = 5;  // Standard duration
    }
};

using SetSun = SetWeather<logic::state::Weather::SUN>;
using SetRain = SetWeather<logic::state::Weather::RAIN>;
using SetSandstorm = SetWeather<logic::state::Weather::SANDSTORM>;
using SetHail = SetWeather<logic::state::Weather::HAIL>;

// ============================================================================
//                              SCREENS
// ============================================================================
//
// ops for setting screens (Reflect, Light Screen, etc.)
//
// Domain: Side
// Stage:  Genesis -> EffectApplied
// ============================================================================

struct SetReflect : CommandMeta<Domain::Side, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        if (ctx.attacker_side->has_reflect()) {
            ctx.result.failed = true;
            return;
        }
        ctx.attacker_side->reflect_turns = 5;
    }
};

struct SetLightScreen : CommandMeta<Domain::Side, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        if (ctx.attacker_side->has_light_screen()) {
            ctx.result.failed = true;
            return;
        }
        ctx.attacker_side->light_screen_turns = 5;
    }
};

struct SetSafeguard : CommandMeta<Domain::Side, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        if (ctx.attacker_side->has_safeguard()) {
            ctx.result.failed = true;
            return;
        }
        ctx.attacker_side->safeguard_turns = 5;
    }
};

struct SetMist : CommandMeta<Domain::Side, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        if (ctx.attacker_side->has_mist()) {
            ctx.result.failed = true;
            return;
        }
        ctx.attacker_side->mist_turns = 5;
    }
};

// ============================================================================
//                              HAZARDS
// ============================================================================

struct AddSpikes : CommandMeta<Domain::Side, Genesis, EffectApplied> {
    static void execute(dsl::BattleContext& ctx) {
        if (ctx.defender_side->spikes_layers >= 3) {
            ctx.result.failed = true;
            return;
        }
        ctx.defender_side->spikes_layers++;
    }
};

}  // namespace logic::ops
