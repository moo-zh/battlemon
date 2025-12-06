#pragma once

#include "../enums/ability.hpp"
#include "../enums/species.hpp"
#include "../enums/stats.hpp"
#include "../enums/type.hpp"

namespace types {

struct Species {
    enums::Species id;
    uint8_t stats[enums::NUM_BASE_STATS];

    enums::Type type1;
    enums::Type type2;
    enums::Ability ability1;
    enums::Ability ability2;
};

}  // namespace types