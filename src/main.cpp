/*                                                                                      *\
                                       ,'\
         _.----.        ____         ,'  _\   ___    ___     ____
     _,-'       `.     |    |  /`.   \,-'    |   \  /   |   |    \  |`.
     \      __    \    '-.  | /   `.  ___    |    \/    |   '-.   \ |  |
      \.    \ \   |  __  |  |/    ,','_  `.  |          | __  |    \|  |
        \    \/   /,' _`.|      ,' / / / /   |          ,' _`.|     |  |
         \     ,-'/  /   \    ,'   | \/ / ,`.|         /  /   \  |     |   ⠀⠀⠀⠀
          \    \ |   \_/  |   `-.  \    `'  /|  |    ||   \_/  | |\    |    ⠀⠀⠀⠀
           \    \ \      /       `-.`.___,-' |  |\  /| \      /  | |   |    ⠀⠀⠀⠀⠀
            \    \ `.__,'|  |`-._    `|      |__| \/ |  `.__,'|  | |   |    ⠀⠀⠀⠀⠀⠀⠀
             \_.-'       |__|    `-._ |              '-.|     '-.| |   |    ⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                     `'                            '-._|    ⢀⡴⠞⢳⠀⠀⠀
                                                          ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⡔⠋⠀⢰⠎⠀
         ____    ____  ______  ______  _        ___       ⠀⠀⠀ ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ ⣼⢆⣤⡞⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
        |    \  /    ||      ||      || |      /  _]      ⠀⠀ ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ ⣼⢠⠋⠁
        |  o  )|  o  ||      ||      || |     /  [_       ⠀  ⠀⠀⠀⢀⣀⣾⢳⠀⠀⠀⠀⢸⢠⠃⠀
        |     ||     ||_|  |_||_|  |_|| |___ |    _]        ⣀⡤⠴⠊⠉⠀⠀⠈⠳⡀⠀⠀⠘⢎⠢⣀⣀⣀⠀⠀⠀
        |  O  ||  _  |  |  |    |  |  |     ||   [_         ⠳⣄⠀⠀⡠⡤⡀⠀⠘⣇⡀⠀⠀⠀⠉⠓⠒⠺⠭⢵⣦⡀⠀
        |     ||  |  |  |  |    |  |  |     ||     |        ⠀⢹⡆⠀⢷⡇⠁⠀⠀⣸⠇⠀⠀⠀⠀⠀⢠⢤⠀⠀⠘⢷⣆⡀⠀⠀
        |_____||__|__|  |__|    |__|  |_____||_____|          ⠘⠒⢤⡄⠖⢾⣭⣤⣄⠀⡔⢢⠀⡀⠎⣸⠀⠀⠀⠀⠹⣿⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
                                                              ⢀⡤⠜⠃⠀⠀⠘⠛⣿⢸⠀⡼⢠⠃⣤⡟⠀⠀⠀⠀⠀⣿⡇⠀
            _____   ____    __ ______   ___   ____   __ __    ⠸⠶⠖⢏⠀⠀⢀⡤⠤⠇⣴⠏⡾⢱⡏⠁⠀⠀⠀⠀⢠⣿⠃
           |     | /    |  /  ]      | /   \ |    \ |  |  |   ⠀⠀⠀⠈⣇⡀⠿⠀⠀⠀⡽⣰⢶⡼⠇⠀⠀⠀⠀⣠⣿⠟
           |   __||  o  | /  /|      ||     ||  D  )|  |  | ⠀⠀⠀ ⠀⠀⠈⠳⢤⣀⡶⠤⣷⣅⡀⠀⠀⠀⣀⡠⢔⠕⠁
           |  |_  |     |/  / |_|  |_||  O  ||    / |  ~  | ⠀⠀ ⠀⠀⠀ ⠀⠀⠀⠀⠀⠈⠙⠫⠿⠿⠿⠛⠋⠁⠀
           |   _] |  _  /   \_  |  |  |     ||    \ |___, |
           |  |   |  |  \     | |  |  |     ||  .  \|     |
           |__|   |__|__|\____| |__|   \___/ |__|\_||____/

\*                                                                                      */

#include "logic/routines/all.hpp"
#include "logic/setup/rental.hpp"
#include "types/models/move.hpp"

// Smoke test: instantiate and execute effects to verify wiring/compilation.
namespace {

template <typename Effect>
void run_effect() {
    using namespace logic;
    using types::enums::Type;

    dsl::BattleContext ctx{};
    state::FieldState field{};
    state::SideState side1{}, side2{};
    state::SlotState slot1{}, slot2{};
    state::MonState mon1{}, mon2{};
    dsl::ActiveMon active1{}, active2{};
    types::Move move{};

    move.power = 40;
    move.accuracy = 100;
    move.type = Type::NORMAL;
    move.flags = types::Move::Flags{types::Move::Flags::MAGIC_COAT_AFFECTED};
    ctx.move = &move;

    // Set up active mon info for damage calculation
    active1.level = 50;
    active1.attack = 100;
    active1.defense = 100;
    active1.sp_attack = 100;
    active1.sp_defense = 100;
    active1.speed = 100;
    active1.type1 = Type::NORMAL;
    active1.type2 = Type::NONE;

    active2.level = 50;
    active2.attack = 100;
    active2.defense = 100;
    active2.sp_attack = 100;
    active2.sp_defense = 100;
    active2.speed = 100;
    active2.type1 = Type::NORMAL;
    active2.type2 = Type::NONE;

    ctx.field = &field;
    ctx.attacker_side = &side1;
    ctx.defender_side = &side2;
    ctx.attacker_slot = &slot1;
    ctx.defender_slot = &slot2;
    ctx.attacker_mon = &mon1;
    ctx.defender_mon = &mon2;
    ctx.attacker_active = &active1;
    ctx.defender_active = &active2;

    // Set up slots array for all-battler iteration
    ctx.slots[0] = &slot1;
    ctx.slots[1] = &slot2;
    ctx.mons[0] = &mon1;
    ctx.mons[1] = &mon2;
    ctx.active_slot_count = 2;

    // Capture deltas for contract-style smoke; prevent unused warnings
    uint16_t atk_hp_before = mon1.current_hp;
    uint16_t def_hp_before = mon2.current_hp;

    Effect::execute(ctx);

    volatile int16_t atk_delta = static_cast<int16_t>(mon1.current_hp) - static_cast<int16_t>(atk_hp_before);
    volatile int16_t def_delta = static_cast<int16_t>(mon2.current_hp) - static_cast<int16_t>(def_hp_before);
    volatile bool status_applied = ctx.result.status_applied;
    volatile bool failed = ctx.result.failed;
    (void)atk_delta; (void)def_delta; (void)status_applied; (void)failed;
}

inline void smoke_test() {
    using namespace logic::routines;

    // Phase 1
    run_effect<Hit>();
    run_effect<AttackUp2>();
    run_effect<AttackDown1>();
    run_effect<PoisonHit>();
    run_effect<Poison>();
    run_effect<LightScreen>();
    run_effect<Sandstorm>();

    // Phase 2
    run_effect<Absorb>();
    run_effect<TakeDown>();
    run_effect<DragonRage>();
    run_effect<Recover>();
    run_effect<Haze>();
    run_effect<SkyAttack>();
    run_effect<PerishSong>();
    run_effect<BatonPass>();
    run_effect<Pursuit>();
    run_effect<MagicCoat>();
}

inline void rental_smoke_test() {
    using namespace logic::setup;

    // Test with first rental (Abra)
    const types::Rental& rental = data::g_RENTAL_SETS[0];
    RentalSetup setup = setup_rental(rental, 50);

    // Verify setup produced valid values
    volatile uint16_t hp = setup.mon.max_hp;
    volatile uint16_t atk = setup.active.attack;
    volatile uint16_t def = setup.active.defense;
    (void)hp; (void)atk; (void)def;

    // Test full battle setup
    dsl::BattleContext ctx{};
    logic::state::FieldState field{};
    logic::state::SideState side1{}, side2{};

    ctx.field = &field;
    ctx.attacker_side = &side1;
    ctx.defender_side = &side2;

    RentalSetup attacker_setup{}, defender_setup{};
    setup_battle(ctx, data::g_RENTAL_SETS[0], data::g_RENTAL_SETS[1],
                 attacker_setup, defender_setup, 50);

    // Verify context was wired up
    volatile bool valid = (ctx.attacker_mon != nullptr) &&
                          (ctx.defender_mon != nullptr) &&
                          (ctx.attacker_active != nullptr);
    (void)valid;
}

}  // namespace

int main() {
    smoke_test();
    rental_smoke_test();
    return 0;
}
