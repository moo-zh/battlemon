#pragma once

#include <cstdint>

namespace types::enums {

enum class Effect : uint8_t {
    // clang-format off

    NONE,

    // === HIT (71) ===
    ABSORB,         ACC_DOWN_HIT, ALL_STATS_UP_HIT, ATK_DOWN_HIT,    ATK_UP_HIT,      
    BEAT_UP,        BLAZE_KICK,   BRICK_BREAK,      BURN_HIT,        CONFUSE_HIT,     
    COUNTER,        DEF_DOWN_HIT, DEF_UP_HIT,       DOUBLE_EDGE,     DOUBLE_HIT,      
    DRAGON_RAGE,    DREAM_EATER,  EARTHQUAKE,       ENDEAVOR,        ERUPTION,        
    EVA_DOWN_HIT,   EXPLOSION,    FACADE,           FAKE_OUT,        FALSE_SWIPE,     
    FLAIL,          FLINCH_HIT,   FLINCH_MINIM_HIT, FREEZE_HIT,      FRUSTRATION,     
    FURY_CUTTER,    GUST,         HIDDEN_POWER,     HIGH_CRITICAL,   HIT,             
    LEVEL_DAMAGE,   LOW_KICK,     MAGNITUDE,        MIRROR_COAT,     MULTI_HIT,       
    OHKO,           PARALYZE_HIT, POISON_FANG,      POISON_HIT,      POISON_TAIL,     
    PRESENT,        PSYWAVE,      PURSUIT,          QUICK_ATTACK,    RECOIL,          
    RECOIL_IF_MISS, RETURN,       REVENGE,          ROLLOUT,         SECRET_POWER,    
    SKY_UPPERCUT,   SMELLINGSALT, SONICBOOM,        SP_ATK_DOWN_HIT, SP_DEF_DOWN_HIT, 
    SPD_DOWN_HIT,   SUPER_FANG,   THAW_HIT,         THUNDER,         TRAP,            
    TRI_ATTACK,     TRIPLE_KICK,  TWINEEDLE,        TWISTER,         VITAL_THROW,     
    WEATHER_BALL,

    // === STAT (41) ===
    ACC_DOWN,      ACC_DOWN_2, ACC_UP,       ACC_UP_2,     ATK_DOWN,      ATK_DOWN_2, ATK_UP,      
    ATK_UP_2,      BELLY_DRUM, BULK_UP,      CALM_MIND,    COSMIC_POWER,  DEF_CURL,   DEF_DOWN,    
    DEF_DOWN_2,    DEF_UP,     DEF_UP_2,     DRAGON_DANCE, EVA_DOWN,      EVA_DOWN_2, EVA_UP,      
    EVA_UP_2,      FLATTER,    FOCUS_ENERGY, HAZE,         MINIMIZE,      PSYCH_UP,   SP_ATK_DOWN, 
    SP_ATK_DOWN_2, SP_ATK_UP,  SP_ATK_UP_2,  SP_DEF_DOWN,  SP_DEF_DOWN_2, SP_DEF_UP,  SP_DEF_UP_2, 
    SPD_DOWN,      SPD_DOWN_2, SPD_UP,       SPD_UP_2,     SWAGGER,       TICKLE,        

    // === STATUS (25) ===
    ATTRACT,     CONFUSE,     CURSE,       DISABLE,   ENCORE, HEAL_BELL, LEECH_SEED, MOONLIGHT,   
    MORNING_SUN, NIGHTMARE,   PAIN_SPLIT,  PARALYZE,  POISON, REFRESH,   REST,       RESTORE_HP,  
    SLEEP,       SOFTBOILED,  SPITE,       SYNTHESIS, TAUNT,  TORMENT,   TOXIC,      WILL_O_WISP, 
    YAWN,        

    // === FIELD (24) ===
    ENDURE,     FOLLOW_ME,   FUTURE_SIGHT, HAIL,    INGRAIN,    KNOCK_OFF,  LIGHT_SCREEN, 
    MAGIC_COAT, MIST,        MUD_SPORT,    PROTECT, RAIN_DANCE, RAPID_SPIN, RECYCLE,      
    REFLECT,    SAFEGUARD,   SANDSTORM,    SNATCH,  SPIKES,     SUBSTITUTE, SUNNY_DAY,    
    TRICK,      WATER_SPORT, WISH,         

    // === COMPOSITE (48) ===
    ALWAYS_HIT,   ASSIST,       BATON_PASS,  BIDE,      CAMOUFLAGE, CHARGE,       CONVERSION,   
    CONVERSION_2, DESTINY_BOND, FOCUS_PUNCH, FORESIGHT, GRUDGE,     HELPING_HAND, IMPRISON,     
    LOCK_ON,      MEAN_LOOK,    MEMENTO,     METRONOME, MIMIC,      MIRROR_MOVE,  NATURE_POWER, 
    OVERHEAT,     PAY_DAY,      PERISH_SONG, RAGE,      RAMPAGE,    RAZOR_WIND,   RECHARGE,     
    ROAR,         ROLE_PLAY,    SEMI_INVUL,  SKETCH,    SKILL_SWAP, SKULL_BASH,   SKY_ATTACK,      
    SLEEP_TALK,   SNORE,        SOLAR_BEAM,  SPIT_UP,   SPLASH,     STOCKPILE,    SUPERPOWER,   
    SWALLOW,      TEETER_DANCE, TELEPORT,    THIEF,     TRANSFORM,  UPROAR

    // clang-format on
};

}  // namespace types::enums
