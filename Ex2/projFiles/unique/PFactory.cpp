#include "PFactory.h"

unique_ptr<Player> PFactory::create(int player_index, size_t x, size_t y,
                                      size_t max_steps, size_t num_shells ) const {
    return make_unique<OurPlayer>(player_index, x, y, max_steps, num_shells);
}