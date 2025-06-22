#ifndef PLAYER_H
#define PLAYER_H

#include "TankAlgorithm.h"
#include "SattelliteView.h"



class Player {
public:
    Player( [[maybe_unused]] int player_index,
            [[maybe_unused]] size_t x, [[maybe_unused]] size_t y,
            [[maybe_unused]] size_t max_steps, [[maybe_unused]] size_t num_shells ) {}
    virtual ~Player() {}
    virtual void updateTankWithBattleInfo
            (TankAlgorithm& tank, SatelliteView& satellite_view) = 0;
};

#endif //ADVANCEDTOPICSINPROGRAMMING_PLAYER_H
