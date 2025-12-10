#pragma once

#include <cstdint>

namespace logic::state {

// ============================================================================
//                               SLOT STATE
// ============================================================================
//
// Domain 3: Per-battle-position state.
// Count: 4 (slots 0,2 for singles; 0,1,2,3 for doubles)
// Lifecycle: CLEARED WHEN POKEMON SWITCHES OUT
//
// This is the critical distinction from MonState. Stat stages, confusion,
// and other volatile effects are properties of the SLOT, not the Pokemon.
// When a Pokemon switches out, it loses these effects.
//
// Exception: Baton Pass transfers specific volatiles to the incoming Pokemon.
// ============================================================================

// Volatile status flags (bitset)
// Note: Use 1ul to ensure 32-bit shifts (eZ80 has 24-bit int)
namespace volatile_flags {
    constexpr uint32_t CONFUSED       = 1ul << 0;
    constexpr uint32_t INFATUATED     = 1ul << 1;
    constexpr uint32_t FOCUS_ENERGY   = 1ul << 2;
    constexpr uint32_t SUBSTITUTE     = 1ul << 3;
    constexpr uint32_t LEECH_SEED     = 1ul << 4;
    constexpr uint32_t CURSED         = 1ul << 5;
    constexpr uint32_t NIGHTMARE      = 1ul << 6;
    constexpr uint32_t TRAPPED        = 1ul << 7;
    constexpr uint32_t WRAPPED        = 1ul << 8;
    constexpr uint32_t TORMENTED      = 1ul << 9;
    constexpr uint32_t DISABLED       = 1ul << 10;
    constexpr uint32_t TAUNTED        = 1ul << 11;
    constexpr uint32_t ENCORED        = 1ul << 12;
    constexpr uint32_t CHARGING       = 1ul << 13;
    constexpr uint32_t SEMI_INVULN    = 1ul << 14;
    constexpr uint32_t DESTINY_BOND   = 1ul << 15;
    constexpr uint32_t GRUDGE         = 1ul << 16;
    constexpr uint32_t INGRAINED      = 1ul << 17;
    constexpr uint32_t YAWN           = 1ul << 18;
    constexpr uint32_t PERISH_SONG    = 1ul << 19;
    constexpr uint32_t LOCK_ON        = 1ul << 20;
    constexpr uint32_t CHARGED        = 1ul << 21;
    constexpr uint32_t DEFENSE_CURL   = 1ul << 22;
    constexpr uint32_t RAGE           = 1ul << 23;
    constexpr uint32_t FORESIGHT      = 1ul << 24;
    constexpr uint32_t BIDE           = 1ul << 25;
    constexpr uint32_t UPROAR         = 1ul << 26;
    constexpr uint32_t TRANSFORMED    = 1ul << 27;
    constexpr uint32_t PROTECTED      = 1ul << 28;
    constexpr uint32_t ENDURED        = 1ul << 29;
    constexpr uint32_t FLINCHED       = 1ul << 30;

    // Flags transferred by Baton Pass
    constexpr uint32_t BATON_PASS_MASK =
        CONFUSED | FOCUS_ENERGY | SUBSTITUTE | LEECH_SEED |
        CURSED | TRAPPED | INGRAINED | PERISH_SONG | LOCK_ON;
}

struct SlotState {
    // Stat stages (-6 to +6, stored as 0-12 with 6 = neutral)
    int8_t atk_stage{6};
    int8_t def_stage{6};
    int8_t spd_stage{6};
    int8_t sp_atk_stage{6};
    int8_t sp_def_stage{6};
    int8_t accuracy_stage{6};
    int8_t evasion_stage{6};

    // Volatile status bitfield
    uint32_t volatiles{0};

    // Volatile counters
    uint8_t confusion_turns{0};
    uint8_t wrap_turns{0};
    uint8_t taunt_turns{0};
    uint8_t encore_turns{0};
    uint8_t disable_turns{0};
    uint8_t perish_count{0};
    uint8_t stockpile_count{0};
    uint8_t fury_cutter_power{0};
    uint8_t rollout_hits{0};
    uint8_t yawn_turns{0};

    // Substitute
    uint16_t substitute_hp{0};

    // Move tracking
    uint8_t disabled_move{0};
    uint8_t encored_move{0};
    uint8_t last_move_used{0};
    uint8_t charging_move{0};

    // Damage tracking (for Counter/Mirror Coat)
    uint16_t physical_damage_taken{0};
    uint16_t special_damage_taken{0};
    uint8_t physical_attacker{0xFF};
    uint8_t special_attacker{0xFF};

    // Relationships
    uint8_t infatuated_with{0xFF};
    uint8_t leech_seed_target{0xFF};
    uint8_t trapped_by{0xFF};

    // Per-turn state
    bool is_first_turn{true};
    bool moved_this_turn{false};
    bool bounce_move{false};  // Magic Coat: reflect eligible status moves

    // Helpers
    constexpr bool has(uint32_t flag) const { return volatiles & flag; }
    constexpr void set(uint32_t flag) { volatiles |= flag; }
    constexpr void clear(uint32_t flag) { volatiles &= ~flag; }

    // Get effective stat stage (-6 to +6)
    constexpr int8_t effective_stage(int8_t raw) const {
        return static_cast<int8_t>(raw - 6);
    }

    // Clear for switch-out (normal)
    constexpr void clear_on_switch() {
        *this = SlotState{};
    }

    // Clear for switch-out (Baton Pass - preserve transferable state)
    constexpr void clear_for_baton_pass() {
        uint32_t preserved_volatiles = volatiles & volatile_flags::BATON_PASS_MASK;
        int8_t preserved_stages[] = {
            atk_stage, def_stage, spd_stage,
            sp_atk_stage, sp_def_stage, accuracy_stage, evasion_stage
        };
        uint16_t preserved_sub_hp = substitute_hp;
        uint8_t preserved_perish = perish_count;
        uint8_t preserved_leech = leech_seed_target;

        *this = SlotState{};

        volatiles = preserved_volatiles;
        atk_stage = preserved_stages[0];
        def_stage = preserved_stages[1];
        spd_stage = preserved_stages[2];
        sp_atk_stage = preserved_stages[3];
        sp_def_stage = preserved_stages[4];
        accuracy_stage = preserved_stages[5];
        evasion_stage = preserved_stages[6];
        substitute_hp = preserved_sub_hp;
        perish_count = preserved_perish;
        leech_seed_target = preserved_leech;
    }

    // Clear per-turn flags (called each turn)
    constexpr void clear_turn_flags() {
        clear(volatile_flags::PROTECTED);
        clear(volatile_flags::ENDURED);
        clear(volatile_flags::FLINCHED);
        physical_damage_taken = 0;
        special_damage_taken = 0;
        physical_attacker = 0xFF;
        special_attacker = 0xFF;
        moved_this_turn = false;
        bounce_move = false;
    }
};

}  // namespace logic::state
