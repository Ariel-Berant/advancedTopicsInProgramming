#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include "PlayerTankAlgorithm.h"


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
    
public:
    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo &info) override;
    Player2TankAlgorithm(int x, int y, orientation orient);
    objMove play(const vector<vector<array<shared_ptr<matrixObject>, 3>>>& gameBoard, const int otherLoc[2], int numOfCols, int numOfRows);
    array<int,4> searchForBullets(const vector<vector<array<shared_ptr<matrixObject>, 3>>> &gameBoard, int inRows, int inCols) const;
};

#endif //PLAYERTANK_H
