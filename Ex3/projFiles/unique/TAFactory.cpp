#include "TAFactory.h"

TAFactory::TAFactory() {
    // Constructor implementation
}

TAFactory::~TAFactory() {
    // Destructor implementation
    // No specific cleanup is needed here since the unique_ptr will automatically clean up the created objects.
}

std::unique_ptr<TankAlgorithm> TAFactory::create(
            int player_index, [[maybe_unused]] int tank_index) const {
    std::unique_ptr<TankAlgorithm> newTankAlgorithm;
    if (player_index == 1) {
        newTankAlgorithm = make_unique<Player1TankAlgorithm>(0, 0, L);
    }
    else{
        newTankAlgorithm = make_unique<Player2TankAlgorithm>(0, 0, R);
    }

    return newTankAlgorithm;
}
