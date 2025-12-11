#include "battle.hpp"

#include "data/move.hpp"
#include "dispatch.hpp"
#include "dsl/turn_pipeline.hpp"
#include "logic/calc/speed.hpp"
#include "util/random.hpp"

namespace engine {

// ============================================================================
//                         INITIALIZATION
// ============================================================================

void BattleEngine::init(const types::Rental& p1_rental, const types::Rental& p2_rental,
                        uint8_t level) {
    level_ = level;
    p1_rental_ = &p1_rental;
    p2_rental_ = &p2_rental;

    p1_setup_ = logic::setup::setup_rental(p1_rental, level);
    p2_setup_ = logic::setup::setup_rental(p2_rental, level);

    ctx_.field = &field_;
    ctx_.attacker_side = &p1_side_;
    ctx_.defender_side = &p2_side_;

    ctx_.slots[0] = &p1_setup_.slot;
    ctx_.slots[1] = &p2_setup_.slot;
    ctx_.mons[0] = &p1_setup_.mon;
    ctx_.mons[1] = &p2_setup_.mon;
    ctx_.active_slot_count = 2;

    set_attacker(0);
}

// ============================================================================
//                         TURN EXECUTION
// ============================================================================

void BattleEngine::execute_turn(const BattleAction& p1_action, const BattleAction& p2_action) {
    // ========================================================================
    // TurnGenesis -> Clear per-turn state
    // ========================================================================
    p1_setup_.slot.clear_turn_flags();
    p2_setup_.slot.clear_turn_flags();

    // ========================================================================
    // TurnGenesis -> PriorityDetermined
    // Fire Quick Claw and other turn-start item events
    // ========================================================================
    bool p1_quick_claw = false;
    bool p2_quick_claw = false;
    dsl::turn::fire_turn_start_for_slot(ctx_, &p1_setup_.slot, p1_quick_claw);
    dsl::turn::fire_turn_start_for_slot(ctx_, &p2_setup_.slot, p2_quick_claw);

    uint8_t first_slot, second_slot;
    const BattleAction* first_action;
    const BattleAction* second_action;
    determine_order(p1_action, p2_action, first_slot, second_slot, first_action, second_action,
                    p1_quick_claw, p2_quick_claw);

    // ========================================================================
    // PriorityDetermined -> ActionsResolving
    // Execute moves in determined order
    // ========================================================================
    execute_action(first_slot, *first_action);

    if (!get_mon(second_slot).is_fainted()) {
        execute_action(second_slot, *second_action);
    }

    // ========================================================================
    // ActionsResolved -> TurnEnd
    // Fire end-of-turn item events (Leftovers, etc.)
    // ========================================================================
    if (!p1_setup_.mon.is_fainted()) {
        dsl::turn::fire_turn_end_for_slot(ctx_, &p1_setup_.slot, &p1_setup_.mon);
    }
    if (!p2_setup_.mon.is_fainted()) {
        dsl::turn::fire_turn_end_for_slot(ctx_, &p2_setup_.slot, &p2_setup_.mon);
    }

    // TODO: Weather damage, poison/burn damage, etc.
}

// ============================================================================
//                         TURN ORDER
// ============================================================================

void BattleEngine::determine_order(const BattleAction& p1_action, const BattleAction& p2_action,
                                   uint8_t& first_slot, uint8_t& second_slot,
                                   const BattleAction*& first_action,
                                   const BattleAction*& second_action, bool p1_quick_claw,
                                   bool p2_quick_claw) {
    int8_t p1_priority = get_action_priority(p1_action, 0);
    int8_t p2_priority = get_action_priority(p2_action, 1);

    auto p1_speed =
        logic::calc::calc_effective_speed(p1_setup_.active, p1_setup_.slot, p1_setup_.mon);
    auto p2_speed =
        logic::calc::calc_effective_speed(p2_setup_.active, p2_setup_.slot, p2_setup_.mon);

    // Quick Claw: if one battler has Quick Claw active and the other doesn't,
    // the Quick Claw user moves first (within same priority bracket)
    logic::calc::TurnOrder order;

    if (p1_priority != p2_priority) {
        // Different priority - Quick Claw doesn't matter
        order = logic::calc::determine_turn_order(p1_priority, p2_priority, p1_speed, p2_speed);
    } else if (p1_quick_claw && !p2_quick_claw) {
        // P1 Quick Claw triggered, P2 didn't -> P1 first
        order = logic::calc::TurnOrder::BATTLER1_FIRST;
    } else if (p2_quick_claw && !p1_quick_claw) {
        // P2 Quick Claw triggered, P1 didn't -> P2 first
        order = logic::calc::TurnOrder::BATTLER2_FIRST;
    } else {
        // Both or neither have Quick Claw -> normal speed comparison
        order = logic::calc::determine_turn_order(p1_priority, p2_priority, p1_speed, p2_speed);
    }

    if (order == logic::calc::TurnOrder::SPEED_TIE) {
        order = (util::random::Random(2) == 0) ? logic::calc::TurnOrder::BATTLER1_FIRST
                                               : logic::calc::TurnOrder::BATTLER2_FIRST;
    }

    if (order == logic::calc::TurnOrder::BATTLER1_FIRST) {
        first_slot = 0;
        second_slot = 1;
        first_action = &p1_action;
        second_action = &p2_action;
    } else {
        first_slot = 1;
        second_slot = 0;
        first_action = &p2_action;
        second_action = &p1_action;
    }
}

int8_t BattleEngine::get_action_priority(const BattleAction& action, uint8_t slot) const {
    if (action.type == BattleAction::Type::MOVE) {
        const auto& rental = get_rental(slot);
        const auto move_id = rental.moves[action.index];
        return lookup_move(move_id).priority;
    }
    // Switches have priority 0 in Gen III (simplified for now)
    return 0;
}

// ============================================================================
//                        ACTION EXECUTION
// ============================================================================

void BattleEngine::execute_action(uint8_t actor_slot, const BattleAction& action) {
    if (action.type == BattleAction::Type::MOVE) {
        execute_move(actor_slot, action.index);
    }
    // TODO: Handle SWITCH and RUN
}

void BattleEngine::execute_move(uint8_t actor_slot, uint8_t move_index) {
    set_attacker(actor_slot);

    const auto& rental = get_rental(actor_slot);
    const auto move_id = rental.moves[move_index];
    const auto& move = lookup_move(move_id);
    ctx_.move = &move;

    ctx_.result = dsl::EffectResult{};
    ctx_.override = dsl::DamageOverride{};

    dispatch_move_effect(move.effect, ctx_);

    auto& slot = get_slot(actor_slot);
    slot.moved_this_turn = true;
    slot.last_move_used = static_cast<uint8_t>(move_id);
}

// ============================================================================
//                           HELPERS
// ============================================================================

void BattleEngine::set_attacker(uint8_t slot) {
    const bool first = (slot == 0);

    auto& atk_setup = first ? p1_setup_ : p2_setup_;
    auto& def_setup = first ? p2_setup_ : p1_setup_;
    auto& atk_side = first ? p1_side_ : p2_side_;
    auto& def_side = first ? p2_side_ : p1_side_;

    const uint8_t atk_id = first ? 0 : 1;
    const uint8_t def_id = first ? 1 : 0;

    ctx_.attacker_slot = &atk_setup.slot;
    ctx_.attacker_mon = &atk_setup.mon;
    ctx_.attacker_active = &atk_setup.active;
    ctx_.attacker_side = &atk_side;
    ctx_.attacker_slot_id = atk_id;
    ctx_.attacker_side_id = atk_id;

    ctx_.defender_slot = &def_setup.slot;
    ctx_.defender_mon = &def_setup.mon;
    ctx_.defender_active = &def_setup.active;
    ctx_.defender_side = &def_side;
    ctx_.defender_slot_id = def_id;
    ctx_.defender_side_id = def_id;
}

const types::Move& BattleEngine::lookup_move(types::enums::Move move_id) {
    return data::g_MOVE_TABLE[static_cast<size_t>(move_id)];
}

}  // namespace engine
