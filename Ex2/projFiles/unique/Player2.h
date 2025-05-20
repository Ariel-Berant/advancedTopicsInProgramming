#ifndef PLAYER2_H
#define PLAYER2_H

#include "OurPlayer.h"
#include "Player2BattleInfo.h"

class Player2 : public OurPlayer{

public:
    Player2();
    ~Player2(); 
    void updateTankWithBattleInfo
            (TankAlgorithm& tank, SatelliteView& satellite_view) override;
    array<int, 3> findClosestEnemy(PlayerTankAlgorithm& tank) const;

};






#endif// Player2.h