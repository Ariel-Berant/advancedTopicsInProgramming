#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include "PlayerTankAlgorithm.h"
#include <memory>


class Player1TankAlgorithm : public PlayerTankAlgorithm {
    ActionRequest playHelper(int numOfRows, int numOfCols, const int *closestEnemyLoc);
public:
    Player1TankAlgorithm(int row, int col, orientation orient);
    ActionRequest getAction() override;
    void updateTurn() override;
    ActionRequest play();
    vector<ActionRequest> playCalc(const int tank2Loc[2], int numOfRows, int numOfCols);
    vector<ActionRequest> handleSurrounded(const int *tank2Loc) const;
};

class Player2TankAlgorithm : public PlayerTankAlgorithm{
private:
    std::unique_ptr<PlayerBattleInfo> battleInfo;
    ActionRequest calculateRun(array<int,4> closestBulletDetails, int numOfCols, int numOfRows, int numOfBulletsChasing);
    ActionRequest calculateNoDangerAction(const int numOfCols, const int numOfRows);
    pair<array<int,4>,int> searchForDangerObjects();
    ActionRequest searchAndDealWithDanger(int numOfCols, int numOfRows);
    ActionRequest noDangerAction(int numOfCols, int numOfRows, const int closestEnemyLoc[2]);
    
public:
    ActionRequest getAction() override;
    Player2TankAlgorithm(int x, int y, orientation orient);
    array<int,4> searchForBullets(int inCols, int inRows) const;
};

#endif //PLAYERTANK_H
