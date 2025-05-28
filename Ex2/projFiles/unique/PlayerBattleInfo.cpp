#include "PlayerBattleInfo.h"

// ...existing code...
PlayerBattleInfo::~PlayerBattleInfo() {
    // default destructor
}

PlayerBattleInfo::PlayerBattleInfo(const PlayerBattleInfo& other)
    : BattleInfo(other)
    , ClosestEnemyTankCol(other.ClosestEnemyTankCol)
    , ClosestEnemyTankRow(other.ClosestEnemyTankRow)
    , TurnsUntillNextUpdate(other.TurnsUntillNextUpdate)
    , TurnsFromLastUpdate(other.TurnsFromLastUpdate)
    , numOfStartingTankBullets(other.numOfStartingTankBullets)
{
}
// ...existing code...