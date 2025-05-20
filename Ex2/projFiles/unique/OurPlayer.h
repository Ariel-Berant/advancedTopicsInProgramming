#ifndef OURPLAYER_H
#define OURPLAYER_H

#include "matrixObject.h"
#include "../common/Player.h"
#include "PlayerTankAlgorithm.h"
using namespace std;




class OurPlayer : public Player
{
protected:
        int player_index;
        size_t x;
        size_t y;
        size_t max_steps;
        size_t num_shells;
        int lastTurnMapUpdated;
        vector<vector<array<shared_ptr<matrixObject>, 3>>> playerGameBoard;
        vector<vector<array<shared_ptr<matrixObject>, 3>>> calculatePlayerGameBoard(SatelliteView& satellite_view, int playerIndex, PlayerTankAlgorithm& tank);
        
public:
    OurPlayer(int player_index,
            size_t x, size_t y,
            size_t max_steps, size_t num_shells);
    ~OurPlayer();   
    virtual void updateTankWithBattleInfo
            (TankAlgorithm& tank, SatelliteView& satellite_view) = 0;
};

#endif// OurPlayer.h