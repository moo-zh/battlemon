#pragma once

#include "../enums/item.hpp"
#include "../enums/move.hpp"
#include "../enums/nature.hpp"
#include "../enums/species.hpp"
#include "../enums/stats.hpp"

namespace types {

struct Rental {
    enums::Species species;
    enums::Move moves[4];
    enums::Item held_item;
    enums::Nature nature;
    enums::EvSpread ev_spread;
    uint8_t ability_slot;
};

}  // namespace types
