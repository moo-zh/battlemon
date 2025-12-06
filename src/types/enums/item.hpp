#pragma once

#include <cstdint>

namespace types::enums {

enum class Item : uint8_t {
    // clang-format off
    
    NONE,

    // ==== Berries (17) ====
    APICOT_B, ASPEAR_B, CHERI_B,  CHESTO_B, GANLON_B, LANSAT_B, LEPPA_B, LIECHI_B, LUM_B, ORAN_B, 
    PECHA_B,  PERSIM_B, PETAYA_B, RAWST_B,  SALAC_B,  SITRUS_B, STARF_B, 

    // ==== Type Boosts (17) ====
    BLACK_BELT,  BLACK_GLASSES, CHARCOAL,     DRAGON_FANG,    HARD_STONE,    MAGNET,     
    METAL_COAT,  MIRACLE_SEED,  MYSTIC_WATER, NEVER_MELT_ICE, POISON_BARB,   SHARP_BEAK, 
    SILK_SCARF,  SILVER_POWDER, SOFT_SAND,    SPELL_TAG,      TWISTED_SPOON, 

    // ==== Signatures (8) ====
    DEEP_SEA_SCALE, DEEP_SEA_TOOTH, LIGHT_BALL, LUCKY_PUNCH, METAL_POWDER, SOUL_DEW, STICK, 
    THICK_CLUB,     

    // ==== Utility (11) ====
    BRIGHT_POWDER, CHOICE_BAND, FOCUS_BAND, KINGS_ROCK, LAX_INCENSE, LEFTOVERS, MENTAL_HERB,
    QUICK_CLAW,    SCOPE_LENS,  SHELL_BELL, WHITE_HERB,

    // clang-format on
};

}  // namespace types::enums