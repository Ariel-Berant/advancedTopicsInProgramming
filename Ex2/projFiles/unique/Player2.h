#ifndef PLAYER2_H
#define PLAYER2_H

#include "OurPlayer.h"
#include "Player2BattleInfo.h"
#include "PlayerTank.h"


class Player2 : public OurPlayer{

public:
    Player2();
    ~Player2(); 
    void updateTankWithBattleInfo
            (TankAlgorithm& tank, SatelliteView& satellite_view) override;
};






#endif// Player2.h