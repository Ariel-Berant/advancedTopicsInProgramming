#include "PFactory.h"

using namespace Algorithm_0000;

class Player1;
class Player2;

std::unique_ptr<Player> PFactory::create(int player_index, size_t x, size_t y,
                                      size_t max_steps, size_t num_shells ) const {
    if (player_index == 1) {
        return std::make_unique<Player1>(player_index, x, y, max_steps, num_shells);
    } else {
        return std::make_unique<Player2>(player_index, x, y, max_steps, num_shells);
    }
    
}