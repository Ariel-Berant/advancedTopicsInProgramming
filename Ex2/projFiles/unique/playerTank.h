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
    objMove play(const vector<vector<array<shared_ptr<matrixObject>, 3>>> &gameBoard, const int otherLoc[2], int numOfCols, int numOfRows);
    vector<objMove> playCalc(const vector<vector<array<shared_ptr<matrixObject>, 3>>> &gameBoard, const int tank2Loc[2], int numOfRows, int numOfCols);
    vector<objMove> handleSurrounded(const vector<vector<array<shared_ptr<matrixObject>, 3>>> &gameBoard, const int *tank2Loc) const;
    bool checkIfOnSameLine(const int *otherLoc) const;
};

class Player2TankAlgorithm : public PlayerTankAlgorithm{
private:
    std::unique_ptr<PlayerBattleInfo> battleInfo;
    objMove calculateRun(array<int,4> closestBulletDetails, int numOfCols, int numOfRows, int numOfBulletsChasing);
    objMove Player2TankAlgorithm::calculateNoDangerAction(const int numOfCols, const int numOfRows);
    
public:
    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo &info) override;
    Player2TankAlgorithm(int x, int y, orientation orient);
    objMove play(int numOfCols, int numOfRows);
    array<int,4> searchForBullets(int inCols, int inRows) const;
};

#endif //PLAYERTANK_H
