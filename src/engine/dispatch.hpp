#pragma once

#include "logic/routines/all.hpp"
#include "logic/state/context.hpp"
#include "types/enums/effect.hpp"

namespace engine {

// ============================================================================
//                           MOVE DISPATCH
// ============================================================================
//
// Dispatches move effects to their corresponding routine implementations.
//
// Development approach: All effects start as stubs pointing to Hit::execute.
// As routines are implemented, the stubs are replaced with the actual routine.
// This allows incremental development while keeping the game functional.
//
// NOTE: No default case - the compiler will warn about missing enum values.
// ============================================================================

/**
 * @brief Execute the effect routine for a given move effect.
 *
 * @param effect The effect enum value from the move data
 * @param ctx The battle context with all state pointers set up
 */
inline void dispatch_move_effect(types::enums::Effect effect, dsl::BattleContext& ctx) {
    using enum types::enums::Effect;
    using namespace logic::routines;

    // clang-format off
    switch (effect) {
        // ====================================================================
        //                         HIT EFFECTS (71)
        // ====================================================================

        case HIT:               Hit::execute(ctx); break;
        case ABSORB:            Absorb::execute(ctx); break;
        case RECOIL:            TakeDown::execute(ctx); break;
        case DRAGON_RAGE:       DragonRage::execute(ctx); break;
        case POISON_HIT:        PoisonHit::execute(ctx); break;

        // Stubs - TODO: implement routines
        case ACC_DOWN_HIT:      Hit::execute(ctx); break;
        case ALL_STATS_UP_HIT:  Hit::execute(ctx); break;
        case ATK_DOWN_HIT:      Hit::execute(ctx); break;
        case ATK_UP_HIT:        Hit::execute(ctx); break;
        case BEAT_UP:           Hit::execute(ctx); break;
        case BLAZE_KICK:        Hit::execute(ctx); break;
        case BRICK_BREAK:       Hit::execute(ctx); break;
        case BURN_HIT:          Hit::execute(ctx); break;
        case CONFUSE_HIT:       Hit::execute(ctx); break;
        case COUNTER:           Hit::execute(ctx); break;
        case DEF_DOWN_HIT:      Hit::execute(ctx); break;
        case DEF_UP_HIT:        Hit::execute(ctx); break;
        case DOUBLE_EDGE:       Hit::execute(ctx); break;
        case DOUBLE_HIT:        Hit::execute(ctx); break;
        case DREAM_EATER:       Hit::execute(ctx); break;
        case EARTHQUAKE:        Hit::execute(ctx); break;
        case ENDEAVOR:          Hit::execute(ctx); break;
        case ERUPTION:          Hit::execute(ctx); break;
        case EVA_DOWN_HIT:      Hit::execute(ctx); break;
        case EXPLOSION:         Hit::execute(ctx); break;
        case FACADE:            Hit::execute(ctx); break;
        case FAKE_OUT:          Hit::execute(ctx); break;
        case FALSE_SWIPE:       Hit::execute(ctx); break;
        case FLAIL:             Hit::execute(ctx); break;
        case FLINCH_HIT:        Hit::execute(ctx); break;
        case FLINCH_MINIM_HIT:  Hit::execute(ctx); break;
        case FREEZE_HIT:        Hit::execute(ctx); break;
        case FRUSTRATION:       Hit::execute(ctx); break;
        case FURY_CUTTER:       Hit::execute(ctx); break;
        case GUST:              Hit::execute(ctx); break;
        case HIDDEN_POWER:      Hit::execute(ctx); break;
        case HIGH_CRITICAL:     Hit::execute(ctx); break;
        case LEVEL_DAMAGE:      Hit::execute(ctx); break;
        case LOW_KICK:          Hit::execute(ctx); break;
        case MAGNITUDE:         Hit::execute(ctx); break;
        case MIRROR_COAT:       Hit::execute(ctx); break;
        case MULTI_HIT:         Hit::execute(ctx); break;
        case OHKO:              Hit::execute(ctx); break;
        case PARALYZE_HIT:      Hit::execute(ctx); break;
        case POISON_FANG:       Hit::execute(ctx); break;
        case POISON_TAIL:       Hit::execute(ctx); break;
        case PRESENT:           Hit::execute(ctx); break;
        case PSYWAVE:           Hit::execute(ctx); break;
        case PURSUIT:           Pursuit::execute(ctx); break;
        case QUICK_ATTACK:      Hit::execute(ctx); break;
        case RECOIL_IF_MISS:    Hit::execute(ctx); break;
        case RETURN:            Hit::execute(ctx); break;
        case REVENGE:           Hit::execute(ctx); break;
        case ROLLOUT:           Hit::execute(ctx); break;
        case SECRET_POWER:      Hit::execute(ctx); break;
        case SKY_UPPERCUT:      Hit::execute(ctx); break;
        case SMELLINGSALT:      Hit::execute(ctx); break;
        case SONICBOOM:         Hit::execute(ctx); break;
        case SP_ATK_DOWN_HIT:   Hit::execute(ctx); break;
        case SP_DEF_DOWN_HIT:   Hit::execute(ctx); break;
        case SPD_DOWN_HIT:      Hit::execute(ctx); break;
        case SUPER_FANG:        Hit::execute(ctx); break;
        case THAW_HIT:          Hit::execute(ctx); break;
        case THUNDER:           Hit::execute(ctx); break;
        case TRAP:              Hit::execute(ctx); break;
        case TRI_ATTACK:        Hit::execute(ctx); break;
        case TRIPLE_KICK:       Hit::execute(ctx); break;
        case TWINEEDLE:         Hit::execute(ctx); break;
        case TWISTER:           Hit::execute(ctx); break;
        case VITAL_THROW:       Hit::execute(ctx); break;
        case WEATHER_BALL:      Hit::execute(ctx); break;

        // ====================================================================
        //                        STAT EFFECTS (41)
        // ====================================================================

        case ATK_UP_2:          AttackUp2::execute(ctx); break;
        case ATK_DOWN:          AttackDown1::execute(ctx); break;
        case HAZE:              Haze::execute(ctx); break;

        // Stubs
        case ACC_DOWN:          Hit::execute(ctx); break;
        case ACC_DOWN_2:        Hit::execute(ctx); break;
        case ACC_UP:            Hit::execute(ctx); break;
        case ACC_UP_2:          Hit::execute(ctx); break;
        case ATK_DOWN_2:        Hit::execute(ctx); break;
        case ATK_UP:            Hit::execute(ctx); break;
        case BELLY_DRUM:        Hit::execute(ctx); break;
        case BULK_UP:           Hit::execute(ctx); break;
        case CALM_MIND:         Hit::execute(ctx); break;
        case COSMIC_POWER:      Hit::execute(ctx); break;
        case DEF_CURL:          Hit::execute(ctx); break;
        case DEF_DOWN:          Hit::execute(ctx); break;
        case DEF_DOWN_2:        Hit::execute(ctx); break;
        case DEF_UP:            Hit::execute(ctx); break;
        case DEF_UP_2:          Hit::execute(ctx); break;
        case DRAGON_DANCE:      Hit::execute(ctx); break;
        case EVA_DOWN:          Hit::execute(ctx); break;
        case EVA_DOWN_2:        Hit::execute(ctx); break;
        case EVA_UP:            Hit::execute(ctx); break;
        case EVA_UP_2:          Hit::execute(ctx); break;
        case FLATTER:           Hit::execute(ctx); break;
        case FOCUS_ENERGY:      Hit::execute(ctx); break;
        case MINIMIZE:          Hit::execute(ctx); break;
        case PSYCH_UP:          Hit::execute(ctx); break;
        case SP_ATK_DOWN:       Hit::execute(ctx); break;
        case SP_ATK_DOWN_2:     Hit::execute(ctx); break;
        case SP_ATK_UP:         Hit::execute(ctx); break;
        case SP_ATK_UP_2:       Hit::execute(ctx); break;
        case SP_DEF_DOWN:       Hit::execute(ctx); break;
        case SP_DEF_DOWN_2:     Hit::execute(ctx); break;
        case SP_DEF_UP:         Hit::execute(ctx); break;
        case SP_DEF_UP_2:       Hit::execute(ctx); break;
        case SPD_DOWN:          Hit::execute(ctx); break;
        case SPD_DOWN_2:        Hit::execute(ctx); break;
        case SPD_UP:            Hit::execute(ctx); break;
        case SPD_UP_2:          Hit::execute(ctx); break;
        case SWAGGER:           Hit::execute(ctx); break;
        case TICKLE:            Hit::execute(ctx); break;

        // ====================================================================
        //                       STATUS EFFECTS (25)
        // ====================================================================

        case POISON:            Poison::execute(ctx); break;
        case RESTORE_HP:        Recover::execute(ctx); break;

        // Stubs
        case ATTRACT:           Hit::execute(ctx); break;
        case CONFUSE:           Hit::execute(ctx); break;
        case CURSE:             Hit::execute(ctx); break;
        case DISABLE:           Hit::execute(ctx); break;
        case ENCORE:            Hit::execute(ctx); break;
        case HEAL_BELL:         Hit::execute(ctx); break;
        case LEECH_SEED:        Hit::execute(ctx); break;
        case MOONLIGHT:         Hit::execute(ctx); break;
        case MORNING_SUN:       Hit::execute(ctx); break;
        case NIGHTMARE:         Hit::execute(ctx); break;
        case PAIN_SPLIT:        Hit::execute(ctx); break;
        case PARALYZE:          Hit::execute(ctx); break;
        case REFRESH:           Hit::execute(ctx); break;
        case REST:              Hit::execute(ctx); break;
        case SLEEP:             Hit::execute(ctx); break;
        case SOFTBOILED:        Hit::execute(ctx); break;
        case SPITE:             Hit::execute(ctx); break;
        case SYNTHESIS:         Hit::execute(ctx); break;
        case TAUNT:             Hit::execute(ctx); break;
        case TORMENT:           Hit::execute(ctx); break;
        case TOXIC:             Hit::execute(ctx); break;
        case WILL_O_WISP:       Hit::execute(ctx); break;
        case YAWN:              Hit::execute(ctx); break;

        // ====================================================================
        //                        FIELD EFFECTS (24)
        // ====================================================================

        case LIGHT_SCREEN:      LightScreen::execute(ctx); break;
        case REFLECT:           Reflect::execute(ctx); break;
        case SANDSTORM:         Sandstorm::execute(ctx); break;
        case SUNNY_DAY:         SunnyDay::execute(ctx); break;
        case RAIN_DANCE:        RainDance::execute(ctx); break;
        case HAIL:              HailEffect::execute(ctx); break;

        // Stubs
        case ENDURE:            Hit::execute(ctx); break;
        case FOLLOW_ME:         Hit::execute(ctx); break;
        case FUTURE_SIGHT:      Hit::execute(ctx); break;
        case INGRAIN:           Hit::execute(ctx); break;
        case KNOCK_OFF:         Hit::execute(ctx); break;
        case MAGIC_COAT:        MagicCoat::execute(ctx); break;
        case MIST:              Hit::execute(ctx); break;
        case MUD_SPORT:         Hit::execute(ctx); break;
        case PROTECT:           Hit::execute(ctx); break;
        case RAPID_SPIN:        Hit::execute(ctx); break;
        case RECYCLE:           Hit::execute(ctx); break;
        case SAFEGUARD:         Hit::execute(ctx); break;
        case SNATCH:            Hit::execute(ctx); break;
        case SPIKES:            Hit::execute(ctx); break;
        case SUBSTITUTE:        Hit::execute(ctx); break;
        case TRICK:             Hit::execute(ctx); break;
        case WATER_SPORT:       Hit::execute(ctx); break;
        case WISH:              Hit::execute(ctx); break;

        // ====================================================================
        //                      COMPOSITE EFFECTS (48)
        // ====================================================================

        case SKY_ATTACK:        SkyAttack::execute(ctx); break;
        case BATON_PASS:        BatonPass::execute(ctx); break;
        case PERISH_SONG:       PerishSong::execute(ctx); break;

        // Stubs
        case ALWAYS_HIT:        Hit::execute(ctx); break;
        case ASSIST:            Hit::execute(ctx); break;
        case BIDE:              Hit::execute(ctx); break;
        case CAMOUFLAGE:        Hit::execute(ctx); break;
        case CHARGE:            Hit::execute(ctx); break;
        case CONVERSION:        Hit::execute(ctx); break;
        case CONVERSION_2:      Hit::execute(ctx); break;
        case DESTINY_BOND:      Hit::execute(ctx); break;
        case FOCUS_PUNCH:       Hit::execute(ctx); break;
        case FORESIGHT:         Hit::execute(ctx); break;
        case GRUDGE:            Hit::execute(ctx); break;
        case HELPING_HAND:      Hit::execute(ctx); break;
        case IMPRISON:          Hit::execute(ctx); break;
        case LOCK_ON:           Hit::execute(ctx); break;
        case MEAN_LOOK:         Hit::execute(ctx); break;
        case MEMENTO:           Hit::execute(ctx); break;
        case METRONOME:         Hit::execute(ctx); break;
        case MIMIC:             Hit::execute(ctx); break;
        case MIRROR_MOVE:       Hit::execute(ctx); break;
        case NATURE_POWER:      Hit::execute(ctx); break;
        case OVERHEAT:          Hit::execute(ctx); break;
        case PAY_DAY:           Hit::execute(ctx); break;
        case RAGE:              Hit::execute(ctx); break;
        case RAMPAGE:           Hit::execute(ctx); break;
        case RAZOR_WIND:        Hit::execute(ctx); break;
        case RECHARGE:          Hit::execute(ctx); break;
        case ROAR:              Hit::execute(ctx); break;
        case ROLE_PLAY:         Hit::execute(ctx); break;
        case SEMI_INVUL:        Hit::execute(ctx); break;
        case SKETCH:            Hit::execute(ctx); break;
        case SKILL_SWAP:        Hit::execute(ctx); break;
        case SKULL_BASH:        Hit::execute(ctx); break;
        case SLEEP_TALK:        Hit::execute(ctx); break;
        case SNORE:             Hit::execute(ctx); break;
        case SOLAR_BEAM:        Hit::execute(ctx); break;
        case SPIT_UP:           Hit::execute(ctx); break;
        case SPLASH:            Hit::execute(ctx); break;
        case STOCKPILE:         Hit::execute(ctx); break;
        case SUPERPOWER:        Hit::execute(ctx); break;
        case SWALLOW:           Hit::execute(ctx); break;
        case TEETER_DANCE:      Hit::execute(ctx); break;
        case TELEPORT:          Hit::execute(ctx); break;
        case THIEF:             Hit::execute(ctx); break;
        case TRANSFORM:         Hit::execute(ctx); break;
        case UPROAR:            Hit::execute(ctx); break;

        // ====================================================================
        //                              NONE
        // ====================================================================

        case NONE: break;    
    }
    // clang-format on
}

}  // namespace engine
