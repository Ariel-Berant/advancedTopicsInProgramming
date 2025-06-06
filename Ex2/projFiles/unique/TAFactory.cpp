#include "TAFactory.h"

std::unique_ptr<TankAlgorithm> create(int player_index, [[maybe_unused]] int tank_index) {
    std::unique_ptr<TankAlgorithm> newTankAlgorithm;
    if (player_index == 1) {
        newTankAlgorithm = make_unique<Player1TankAlgorithm>(0, 0, L);
    }
    else{
        newTankAlgorithm = make_unique<Player2TankAlgorithm>(0, 0, R);
    }

    return newTankAlgorithm;
}
