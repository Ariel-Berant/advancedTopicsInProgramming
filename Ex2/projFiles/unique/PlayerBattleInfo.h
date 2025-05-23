#ifndef PLAYERBATTLEINFO_H
#define PLAYERBATTLEINFO_H

#include "../common/BattleInfo.h"


class PlayerBattleInfo : public BattleInfo{
private:
    vector<vector<array<shared_ptr<matrixObject>, 3>>> tankGameBoard;
    const size_t ClosestEnemyTankCol;
    const size_t ClosestEnemyTankRow;
    int TurnsUntillNextUpdate;
    int TurnsFromLastUpdate;

public:
    PlayerBattleInfo(size_t ClosestEnemyTankCol, size_t ClosestEnemyTankRow, int turnsUntillNextUpdate, 
                                         vector<vector<array<shared_ptr<matrixObject>, 3>>> &tankGameBoard)
        : tankGameBoard(tankGameBoard), ClosestEnemyTankCol(ClosestEnemyTankCol), ClosestEnemyTankRow(ClosestEnemyTankRow),
         TurnsUntillNextUpdate(turnsUntillNextUpdate), TurnsFromLastUpdate(0) {
    }
    ~PlayerBattleInfo();
    size_t getClosestEnemyTankCol() const { return ClosestEnemyTankCol; }
    size_t getClosestEnemyTankRow() const { return ClosestEnemyTankRow; }
    int getTurnsUntillNextUpdate() const { return TurnsUntillNextUpdate; }
    void setTurnsUntillNextUpdate() { TurnsUntillNextUpdate--; }
    void setTurnsFromLastUpdate() { TurnsFromLastUpdate++; }
    int getTurnsFromLastUpdate() const { return TurnsFromLastUpdate; }
    vector<vector<array<shared_ptr<matrixObject>, 3>>> getGameBoard() const { return tankGameBoard; }
        array<int, 3> findClosestEnemy(PlayerTankAlgorithm& tank) const;
};

#endif// PlayerBattleInfo.