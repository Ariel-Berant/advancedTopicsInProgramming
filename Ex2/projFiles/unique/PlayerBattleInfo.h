#ifndef PLAYERBATTLEINFO_H
#define PLAYERBATTLEINFO_H

#include "../common/BattleInfo.h"


class PlayerBattleInfo : public BattleInfo{
private:
    vector<vector<array<shared_ptr<matrixObject>, 3>>> tankGameBoard;
    const size_t ClosestEnemyTankX;
    const size_t ClosestEnemyTankY;
    int TurnsUntillNextUpdate;

public:
    PlayerBattleInfo(size_t closestEnemyTankX, size_t closestEnemyTankY, int turnsUntillNextUpdate, 
                                         vector<vector<array<shared_ptr<matrixObject>, 3>>> &tankGameBoard)
        : tankGameBoard(tankGameBoard), ClosestEnemyTankX(closestEnemyTankX), ClosestEnemyTankY(closestEnemyTankY),
         TurnsUntillNextUpdate(turnsUntillNextUpdate) {
    }
    ~PlayerBattleInfo();
    size_t getClosestEnemyTankX() const { return ClosestEnemyTankX; }
    size_t getClosestEnemyTankY() const { return ClosestEnemyTankY; }
    int getTurnsUntillNextUpdate() const { return TurnsUntillNextUpdate; }
    void setTurnsUntillNextUpdate() { TurnsUntillNextUpdate--; }
        array<int, 3> findClosestEnemy(PlayerTankAlgorithm& tank) const;
};

#endif// PlayerBattleInfo.