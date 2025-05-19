#ifndef PLAYER2BATTLEINFO_H
#define PLAYER2BATTLEINFO_H

#include "PlayerBattleInfo.h"
#include <cstddef>



class Player2BattleInfo : PlayerBattleInfo{
private:
    const size_t ClosestEnemyTankX;
    const size_t ClosestEnemyTankY;
    int TurnsUntillNextUpdate;
public:
    Player2BattleInfo::Player2BattleInfo(size_t closestEnemyTankX, size_t closestEnemyTankY, int turnsUntillNextUpdate)
    : ClosestEnemyTankX(closestEnemyTankX), ClosestEnemyTankY(closestEnemyTankY), TurnsUntillNextUpdate(turnsUntillNextUpdate) {
    }

    Player2BattleInfo(const Player2BattleInfo& other) = delete; // Disable copy constructor
    Player2BattleInfo& operator=(const Player2BattleInfo& other) = delete; // Disable copy assignment operator

    ~Player2BattleInfo();
    size_t getClosestEnemyTankX() const { return ClosestEnemyTankX; }
    size_t getClosestEnemyTankY() const { return ClosestEnemyTankY; }
    int getTurnsUntillNextUpdate() const { return TurnsUntillNextUpdate; }
    void setTurnsUntillNextUpdate() { TurnsUntillNextUpdate--; }


};

#endif// PlayerBattleInfo.
