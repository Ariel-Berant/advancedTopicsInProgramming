#include "Players.h"

REGISTER_PLAYER(Player2);

using namespace UserCommon_0000;

using namespace Algorithm_0000;
    
Player2::Player2(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
    : OurPlayer(player_index, x, y, max_steps, num_shells) {
}

Player2::~Player2() = default;