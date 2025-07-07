#ifndef PLAYERBATTLEINFO_H
#define PLAYERBATTLEINFO_H

#include "../common/BattleInfo.h"
#include "../UserCommon/MatrixObject.h"

using namespace std;

class PlayerTankAlgorithm; // Forward declaration

class PlayerBattleInfo : public BattleInfo{
private:
    vector<vector<array<shared_ptr<matrixObject>, 2>>> tankGameBoard;
    const int ClosestEnemyTankCol;
    const int ClosestEnemyTankRow;
    int TurnsUntillNextUpdate;
    int TurnsFromLastUpdate;
    int numOfStartingTankBullets;

public:
    PlayerBattleInfo(int ClosestEnemyTankCol, int ClosestEnemyTankRow, int turnsUntillNextUpdate, 
                                         vector<vector<array<shared_ptr<matrixObject>, 2>>> &tankGameBoard, int numOfStartingTankBullets)
        : tankGameBoard(tankGameBoard), ClosestEnemyTankCol(ClosestEnemyTankCol), ClosestEnemyTankRow(ClosestEnemyTankRow),
         TurnsUntillNextUpdate(turnsUntillNextUpdate), TurnsFromLastUpdate(0), numOfStartingTankBullets(numOfStartingTankBullets) {
    }
    ~PlayerBattleInfo();
    PlayerBattleInfo(const PlayerBattleInfo& other);
    int getClosestEnemyTankCol() const { return ClosestEnemyTankCol; }
    int getClosestEnemyTankRow() const { return ClosestEnemyTankRow; }
    int getTurnsUntillNextUpdate() const { return TurnsUntillNextUpdate; }
    void setTurnsUntillNextUpdate() { TurnsUntillNextUpdate--; }
    void setTurnsFromLastUpdate() { TurnsFromLastUpdate++; }
    void resetTurnsFromLastUpdate() { TurnsFromLastUpdate = 0; }
    int getTurnsFromLastUpdate() const { return TurnsFromLastUpdate; }
    int getNumOfStartingTankBullets() const { return numOfStartingTankBullets; }
    vector<vector<array<shared_ptr<matrixObject>, 2>>> getGameBoard() const { return tankGameBoard; }
        array<int, 3> findClosestEnemy(PlayerTankAlgorithm& tank) const;
};

#endif// PlayerBattleInfo.