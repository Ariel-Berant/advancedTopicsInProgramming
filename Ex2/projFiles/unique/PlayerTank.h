#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include "PlayerTankAlgorithm.h"
#include "Player2BattleInfo.h"
#include <memory>


class Player1TankAlgorithm : public PlayerTankAlgorithm {
public:
    Player1TankAlgorithm(int row, int col, orientation orient);
    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo &info) override;
    void updateTurn() override;
    ActionRequest play(int numOfCols, int numOfRows);
    vector<ActionRequest> playCalc(const int tank2Loc[2], int numOfRows, int numOfCols);
    vector<ActionRequest> handleSurrounded(const int *tank2Loc) const;
    bool checkIfOnSameLine(const int *otherLoc) const;
};

class Player2TankAlgorithm : public PlayerTankAlgorithm{
private:
    std::unique_ptr<PlayerBattleInfo> battleInfo;
    ActionRequest calculateRun(array<int,4> closestBulletDetails, int numOfCols, int numOfRows, int numOfBulletsChasing);
    ActionRequest Player2TankAlgorithm::calculateNoDangerAction(const int numOfCols, const int numOfRows);
    pair<array<int,4>,int> searchForDangerObjects();
    
public:
    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo &info) override;
    Player2TankAlgorithm(int x, int y, orientation orient);
    array<int,4> searchForBullets(int inCols, int inRows) const;
};

#endif //PLAYERTANK_H
