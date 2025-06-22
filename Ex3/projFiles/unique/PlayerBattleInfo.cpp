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
    // Deep copy tankGameBoard
    tankGameBoard.resize(other.tankGameBoard.size());
    for (size_t i = 0; i < other.tankGameBoard.size(); i++) {
        tankGameBoard[i].resize(other.tankGameBoard[i].size());
        for (size_t j = 0; j < other.tankGameBoard[i].size(); j++) {
            for (size_t k = 0; k < 2; k++) {
                if (other.tankGameBoard[i][j][k]) {
                    tankGameBoard[i][j][k] = std::make_shared<matrixObject>(*other.tankGameBoard[i][j][k]);
                }
                else{
                    tankGameBoard[i][j][k] = nullptr; // Ensure null pointers are preserved
                }
            }
        }
    }

}

