#include "OurPlayer.h"


OurPlayer::OurPlayer(int player_index, size_t x, size_t y,
     size_t max_steps, size_t num_shells)
        : Player(player_index, x, y, max_steps, num_shells), // Call base class constructor
          player_index(player_index), x(x), y(y), max_steps(max_steps), num_shells(num_shells), lastTurnMapUpdated(-1){


}



// need to continue the implementation of this function (deduce directions, find a way to update only if a turn has been made)
vector<vector<array<shared_ptr<matrixObject>, 3>>> OurPlayer::calculatePlayerGameBoard(SatelliteView& satellite_view, int playerIndex, PlayerTankAlgorithm& tank) {
    if(tank.getNumOfShotsLeft() == -1){
        tank.setNumOfShotsLeft(num_shells);
    }
    if(tank.getCurrTurn() == lastTurnMapUpdated){
        for(movingObject & pTank : playerTanks){
            if(satellite_view.getObjectAt(pTank.getLocation()[0], pTank.getLocation()[1]) == '%'){
                tank.setLocation(pTank.getLocation()[0], pTank.getLocation()[1]);
                break;
            }
        }
        return playerGameBoard;
    }
    int currCol = 0, currRow = 0;
    char objType = satellite_view.getObjectAt(currCol, currRow);


    while (objType != '&') {
        while (objType != '&'){
            objType = satellite_view.getObjectAt(currCol, currRow);
            playerGameBoard[currCol][currRow] = {nullptr, nullptr, nullptr};
            if (objType == ' ') {
                continue;
            }
            else if (objType == '#') {
                playerGameBoard[currCol][currRow][0] = make_shared<matrixObject>(currCol, currRow, W);
            }
            else if (objType == '@') {
                playerGameBoard[currCol][currRow][0] = make_shared<matrixObject>(currCol, currRow, M);
            }
            else if (objType == '1') {
                playerGameBoard[currCol][currRow][1] = make_shared<matrixObject>(currCol, currRow, P1T);
                player_index == 1  ?  playerTanks.emplace_back(playerGameBoard[currCol][currRow][1]) : enemysTanks.emplace_back(playerGameBoard[currCol][currRow][1]);
            }
            else if (objType == '2') {
                playerGameBoard[currCol][currRow][1] = make_shared<matrixObject>(currCol, currRow, P2T);
                player_index == 1  ?  playerTanks.emplace_back(playerGameBoard[currCol][currRow][1]) : enemysTanks.emplace_back(playerGameBoard[currCol][currRow][1]);

            }
            else if (objType == '*') {
                playerGameBoard[currCol][currRow][1] = make_shared<matrixObject>(currCol, currRow, B);
            }
            else{
                objectType oType = player_index == 1 ? P1T : P2T;
                playerGameBoard[currCol][currRow][1] = make_shared<matrixObject>(currCol, currRow, oType);
                playerTanks.emplace_back(playerGameBoard[currCol][currRow][1]);
                tank.setLocation(currCol, currRow);
            }
            currRow++;
            objType = satellite_view.getObjectAt(currCol, currRow);
        }
        currCol++;
        currRow = 0;
        objType = satellite_view.getObjectAt(currCol, currRow);
    }
    lastTurnMapUpdated = tank.getCurrTurn();
    return playerGameBoard;
}


void OurPlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    array<int, 3> closestEnemy;
    
    // Update the tank's game board with the satellite view
    auto playerGameBoard = calculatePlayerGameBoard(satellite_view, 2, dynamic_cast<PlayerTankAlgorithm&>(tank));
    closestEnemy = findClosestEnemy(dynamic_cast<PlayerTankAlgorithm&>(tank));
    PlayerBattleInfo tankBattleInfo(closestEnemy[0], closestEnemy[1], closestEnemy[2], playerGameBoard); // Initialize the battle info with the closest enemy coordinates and turns until next update
    tank.updateBattleInfo(tankBattleInfo); // Update the tank's battle info
}

array<int, 3> OurPlayer::findClosestEnemy(PlayerTankAlgorithm& tank) const {
    int enemyCol = -1;
    int enemyRow = -1;
    int minDistance = INT_MAX;
    array<int, 3> closestEnemy = {-1, -1, -1}; // Initialize closest enemy coordinates and distance
    for(movingObject enemy : enemysTanks){
        enemyCol = enemy.getLocation()[0];
        enemyRow = enemy.getLocation()[1];
        int distance = abs(tank.getLocation()[0] - enemyCol) + abs(tank.getLocation()[1] - enemyRow); // Calculate the Manhattan distance
        if(distance < minDistance) {
            minDistance = distance; // Update the minimum distance
            closestEnemy[0] = enemyCol; // Update the closest enemy coordinates
            closestEnemy[1] = enemyRow;
            closestEnemy[2] = distance; // Update the distance
        }        
    }
    return closestEnemy;
} 
