#pragma once

#include "../../dsl/effect.hpp"

namespace logic::routines {

// ============================================================================
//                           FIELD EFFECTS
// ============================================================================
//
// Effects that modify field state (weather) or side state (screens).
// Domain: Field or Screen (Side)
// ============================================================================

using namespace dsl;
using namespace dsl::domains;

// ----------------------------------------------------------------------------
// LIGHT_SCREEN - Set Light Screen on user's side
// ----------------------------------------------------------------------------

EFFECT(LightScreen, Screen) {
    BEGIN(ctx)
    RUN(SetLightScreen)
    END;
}

// ----------------------------------------------------------------------------
// REFLECT - Set Reflect on user's side
// ----------------------------------------------------------------------------

EFFECT(Reflect, Screen) {
    BEGIN(ctx)
    RUN(SetReflect)
    END;
}

// ----------------------------------------------------------------------------
// SANDSTORM - Set Sandstorm weather
// ----------------------------------------------------------------------------

EFFECT(Sandstorm, Domain::Field) {
    BEGIN(ctx)
    RUN(SetSandstorm)
    END;
}

// ----------------------------------------------------------------------------
// SUNNY_DAY - Set Sun weather
// ----------------------------------------------------------------------------

EFFECT(SunnyDay, Domain::Field) {
    BEGIN(ctx)
    RUN(SetSun)
    END;
}

// ----------------------------------------------------------------------------
// RAIN_DANCE - Set Rain weather
// ----------------------------------------------------------------------------

EFFECT(RainDance, Domain::Field) {
    BEGIN(ctx)
    RUN(SetRain)
    END;
}

// ----------------------------------------------------------------------------
// HAIL - Set Hail weather
// ----------------------------------------------------------------------------

EFFECT(HailEffect, Domain::Field) {
    BEGIN(ctx)
    RUN(SetHail)
    END;
}

}  // namespace logic::routines
