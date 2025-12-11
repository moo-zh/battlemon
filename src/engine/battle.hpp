#pragma once

#include <cstdint>

#include "logic/setup/rental.hpp"
#include "logic/state/context.hpp"
#include "logic/state/field.hpp"
#include "logic/state/side.hpp"
#include "types/models/rental.hpp"

namespace engine {

// ============================================================================
//                            BATTLE ACTION
// ============================================================================
//
// Represents an action a battler can take during a turn.
// For Milestone 1, we only support MOVE actions.
// ============================================================================

enum class BattleResult : uint8_t {
    P1_WINS = 0,
    P2_WINS = 1,
    ONGOING = 0xFF,
};

struct BattleAction {
    enum class Type : uint8_t {
        MOVE,    // Use a move (index 0-3)
        SWITCH,  // Switch to party member (index 0-5)
        RUN,     // Attempt to flee (Battle Factory: not applicable)
    };

    Type type{Type::MOVE};
    uint8_t index{0};  // Move index (0-3) or party index for switch

    static constexpr BattleAction move(uint8_t move_index) { return {Type::MOVE, move_index}; }

    static constexpr BattleAction switch_to(uint8_t party_index) {
        return {Type::SWITCH, party_index};
    }
};

// ============================================================================
//                            BATTLE ENGINE
// ============================================================================
//
// Core battle engine for executing turns in a singles battle.
//
// Responsibilities:
//   - Determine turn order (priority, speed, random tiebreak)
//   - Execute actions in order
//   - Dispatch moves to effect routines
//   - Handle end-of-turn effects (TODO: Milestone 2)
//
// ============================================================================

class BattleEngine {
   public:
    // ========================================================================
    //                         INITIALIZATION
    // ========================================================================

    BattleEngine() = default;

    /**
     * @brief Initialize a battle between two rental Pokemon.
     *
     * @param p1_rental Player 1's rental data
     * @param p2_rental Player 2's rental data
     * @param level Battle level (default 50)
     */
    void init(const types::Rental& p1_rental, const types::Rental& p2_rental, uint8_t level = 50);

    // ========================================================================
    //                         TURN EXECUTION
    // ========================================================================

    /**
     * @brief Execute a full turn with both players' actions.
     *
     * Determines turn order and executes actions sequentially.
     *
     * @param p1_action Player 1's action
     * @param p2_action Player 2's action
     */
    void execute_turn(const BattleAction& p1_action, const BattleAction& p2_action);

    // ========================================================================
    //                         STATE ACCESSORS
    // ========================================================================

    [[nodiscard]] const logic::state::MonState& p1_mon() const { return p1_setup_.mon; }
    [[nodiscard]] const logic::state::MonState& p2_mon() const { return p2_setup_.mon; }
    [[nodiscard]] logic::state::MonState& p1_mon() { return p1_setup_.mon; }
    [[nodiscard]] logic::state::MonState& p2_mon() { return p2_setup_.mon; }

    [[nodiscard]] const logic::state::SlotState& p1_slot() const { return p1_setup_.slot; }
    [[nodiscard]] const logic::state::SlotState& p2_slot() const { return p2_setup_.slot; }

    [[nodiscard]] const dsl::ActiveMon& p1_active() const { return p1_setup_.active; }
    [[nodiscard]] const dsl::ActiveMon& p2_active() const { return p2_setup_.active; }

    [[nodiscard]] const dsl::BattleContext& context() const { return ctx_; }
    [[nodiscard]] dsl::BattleContext& context() { return ctx_; }

    [[nodiscard]] BattleResult result() const {
        if (p1_setup_.mon.is_fainted())
            return BattleResult::P2_WINS;
        if (p2_setup_.mon.is_fainted())
            return BattleResult::P1_WINS;
        return BattleResult::ONGOING;
    }

   private:
    // ========================================================================
    //                         TURN ORDER
    // ========================================================================

    void determine_order(const BattleAction& p1_action, const BattleAction& p2_action,
                         uint8_t& first_slot, uint8_t& second_slot,
                         const BattleAction*& first_action, const BattleAction*& second_action,
                         bool p1_quick_claw, bool p2_quick_claw);

    [[nodiscard]] int8_t get_action_priority(const BattleAction& action, uint8_t slot) const;

    // ========================================================================
    //                        ACTION EXECUTION
    // ========================================================================

    void execute_action(uint8_t actor_slot, const BattleAction& action);
    void execute_move(uint8_t actor_slot, uint8_t move_index);

    // ========================================================================
    //                           HELPERS
    // ========================================================================

    void set_attacker(uint8_t slot);

    [[nodiscard]] logic::state::MonState& get_mon(uint8_t slot) {
        return (slot == 0) ? p1_setup_.mon : p2_setup_.mon;
    }

    [[nodiscard]] logic::state::SlotState& get_slot(uint8_t slot) {
        return (slot == 0) ? p1_setup_.slot : p2_setup_.slot;
    }

    [[nodiscard]] const types::Rental& get_rental(uint8_t slot) const {
        return (slot == 0) ? *p1_rental_ : *p2_rental_;
    }

    [[nodiscard]] static const types::Move& lookup_move(types::enums::Move move_id);

    // ========================================================================
    //                             STATE
    // ========================================================================

    dsl::BattleContext ctx_{};
    logic::state::FieldState field_{};
    logic::state::SideState p1_side_{};
    logic::state::SideState p2_side_{};

    logic::setup::RentalSetup p1_setup_{};
    logic::setup::RentalSetup p2_setup_{};

    const types::Rental* p1_rental_{nullptr};
    const types::Rental* p2_rental_{nullptr};

    uint8_t level_{50};
};

}  // namespace engine
