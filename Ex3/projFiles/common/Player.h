#pragma once

#include "TankAlgorithm.h"
#include "SatelliteView.h"
#include <functional>



class Player {
public:
    Player( [[maybe_unused]] int player_index,
            [[maybe_unused]] size_t x, [[maybe_unused]] size_t y,
            [[maybe_unused]] size_t max_steps, [[maybe_unused]] size_t num_shells ) {}
    virtual ~Player() {}
    virtual void updateTankWithBattleInfo
            (TankAlgorithm& tank, SatelliteView& satellite_view) = 0;
};

using PlayerFactory =
        std::function<std::unique_ptr<Player>
                (int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)>;

