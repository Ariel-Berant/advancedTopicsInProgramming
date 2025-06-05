#include "TAFactory.h"

std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) {
    std::unique_ptr<TankAlgorithm> newTankAlgorithm;
    if (player_index == 1) {
        newTankAlgorithm = make_unique<Player1TankAlgorithm>(nullptr, nullptr, L);
    }
    else{
        newTankAlgorithm = make_unique<Player2TankAlgorithm>(nullptr, nullptr, R);
    }

    return newTankAlgorithm;
}
