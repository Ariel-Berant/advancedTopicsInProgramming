#ifndef OURPLAYER_H
#define OURPLAYER_H

#include "../UserCommon/MatrixObject.h"
#include "../common/Player.h"
#include "../common/PlayerRegistration.h"
#include "PlayerTankAlgorithm.h"
#include "PlayerTank.h"
#include "PlayerBattleInfo.h"
#include <climits>

using namespace UserCommon_0000;

namespace Algorithm_0000 {
        class OurPlayer : public Player
        {
        protected:
                int player_index;
                size_t x;
                size_t y;
                size_t max_steps;
                size_t num_shells;
                int lastTurnMapUpdated;
                vector<vector<array<shared_ptr<matrixObject>, 2>>> playerGameBoard;
                vector<shared_ptr<movingObject>> enemysTanks;
                vector<shared_ptr<movingObject>> playerTanks;
                void calculatePlayerGameBoard(SatelliteView& satellite_view, PlayerTankAlgorithm& tank);
                void buildPlayerGameBoard(SatelliteView& satellite_view, PlayerTankAlgorithm& tank);

                
                array<int, 3> findClosestEnemy(PlayerTankAlgorithm& tank) const;

        public:
        OurPlayer(int player_index,
                size_t x, size_t y,
                size_t max_steps, size_t num_shells);
        ~OurPlayer() = default;   
        void updateTankWithBattleInfo
                (TankAlgorithm& tank, SatelliteView& satellite_view) override;
        };
}
#endif// OurPlayer.h