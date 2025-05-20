#include "Player2.h"
#include <climits>


void Player2::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    array<int, 3> closestEnemy;

    // Update the tank's game board with the satellite view
    auto playerGameBoard = calculatePlayerGameBoard(satellite_view, 2, dynamic_cast<PlayerTankAlgorithm&>(tank));
    closestEnemy = findClosestEnemy(dynamic_cast<PlayerTankAlgorithm&>(tank));
    Player2BattleInfo tankBattleInfo = Player2BattleInfo(closestEnemy[0], closestEnemy[1], 3, playerGameBoard); // Initialize the battle info with the closest enemy coordinates and turns until next update
    tank.updateBattleInfo(tankBattleInfo); // Update the tank's battle info
}

array<int, 3> Player2::findClosestEnemy(PlayerTankAlgorithm& tank) const {
    int enemyCol = -1;
    int enemyRow = -1;
    int numOfColumns = playerGameBoard.size();
    int numOfRows = playerGameBoard[0].size();
    int minDistance = INT_MAX;
    array<int, 3> closestEnemy = {-1, -1, -1}; // Initialize closest enemy coordinates and distance
    matrixObject* currObject = nullptr;

    for(int i = 0; i < numOfColumns; i++) {
        for(int j = 0; j < numOfRows; j++) {
            currObject = playerGameBoard[i][j][1].get(); // Get the object in the cell
            if(currObject != nullptr && currObject->getType() == P1T) { // Check if the cell contains an enemy tank
                enemyCol = i;
                enemyRow = j;
                int distance = abs(tank.getLocation()[0] - enemyCol) + abs(tank.getLocation()[1] - enemyRow); // Calculate the Manhattan distance
                if(distance < minDistance) {
                    minDistance = distance; // Update the minimum distance
                    closestEnemy[0] = enemyCol; // Update the closest enemy coordinates
                    closestEnemy[1] = enemyRow;
                    closestEnemy[2] = distance; // Update the distance
                }
            }
        }
    }
    return closestEnemy;
} 