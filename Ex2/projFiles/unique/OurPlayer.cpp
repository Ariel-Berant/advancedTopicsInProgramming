#include "OurPlayer.h"


OurPlayer::OurPlayer(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
        : Player(player_index, x, y, max_steps, num_shells), // Call base class constructor
          player_index(player_index), x(x), y(y), max_steps(max_steps), num_shells(num_shells), lastTurnMapUpdated(-1){
            playerGameBoard = vector<vector<array<shared_ptr<matrixObject>, 2>>>(x, 
                vector<array<shared_ptr<matrixObject>, 2>>(y, {nullptr, nullptr}));
}

void OurPlayer::buildPlayerGameBoard(SatelliteView& satellite_view, PlayerTankAlgorithm& tank) {
    int currCol = 0, currRow = 0;
    char objType = satellite_view.getObjectAt(currCol, currRow);
    while (objType != '&') {
        while (objType != '&'){
            objType = satellite_view.getObjectAt(currCol, currRow);
            playerGameBoard[currCol][currRow] = {nullptr, nullptr};
            if (objType == '#') {
                playerGameBoard[currCol][currRow][0] = make_shared<matrixObject>(currRow, currCol, W);
            }
            else if (objType == '@') {
                playerGameBoard[currCol][currRow][0] = make_shared<matrixObject>(currRow, currCol, M);
            }
            else if (objType == '1') {
                playerGameBoard[currCol][currRow][1] = make_shared<movingObject>(currRow, currCol, P1T, UNKNOWN);
                player_index == 1  ?  playerTanks.emplace_back(dynamic_pointer_cast<movingObject>(playerGameBoard[currCol][currRow][1])) 
                                    : enemysTanks.emplace_back(dynamic_pointer_cast<movingObject>(playerGameBoard[currCol][currRow][1]));
            }
            else if (objType == '2') {
                playerGameBoard[currCol][currRow][1] = make_shared<movingObject>(currRow, currCol, P2T, UNKNOWN);
                player_index == 2  ?  playerTanks.emplace_back(dynamic_pointer_cast<movingObject>(playerGameBoard[currCol][currRow][1])) 
                                    : enemysTanks.emplace_back(dynamic_pointer_cast<movingObject>(playerGameBoard[currCol][currRow][1]));

            }
            else if (objType == '*') {
                playerGameBoard[currCol][currRow][1] = make_shared<movingObject>(currRow, currCol, B, UNKNOWN);
            }
            else if(objType == '%') {
                objectType oType = player_index == 1 ? P1T : P2T;
                playerGameBoard[currCol][currRow][1] = make_shared<movingObject>(currRow, currCol, oType, UNKNOWN);
                playerTanks.emplace_back(dynamic_pointer_cast<movingObject>(playerGameBoard[currCol][currRow][1]));
                tank.setLocation(currCol, currRow);
            }
            currRow++;
            objType = satellite_view.getObjectAt(currCol, currRow);
        }
        currCol++;
        currRow = 0;
        objType = satellite_view.getObjectAt(currCol, currRow);
    }
}


// need to continue the implementation of this function (deduce directions, find a way to update only if a turn has been made)
void OurPlayer::calculatePlayerGameBoard(SatelliteView& satellite_view, PlayerTankAlgorithm& tank) {
    if(tank.getNumOfShotsLeft() == -1){
        tank.setNumOfShotsLeft(num_shells);
    }
    if(tank.getCurrTurn() == lastTurnMapUpdated){
        for(shared_ptr<movingObject> pTank : playerTanks){
            if(satellite_view.getObjectAt(pTank->getLocation()[0], pTank->getLocation()[1]) == '%'){
                bool locationChanged = pTank->getLocation()[0] != tank.getLocation()[0] || pTank->getLocation()[1] != tank.getLocation()[1];
                if(locationChanged){
                    tank.setLocation(pTank->getLocation()[0], pTank->getLocation()[1]);
                }
                break;
            }
        }
    }
    else{
        buildPlayerGameBoard(satellite_view, tank);
        lastTurnMapUpdated = tank.getCurrTurn();
    }
}


void OurPlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    PlayerTankAlgorithm & tankRef = dynamic_cast<PlayerTankAlgorithm&>(tank); // Ensure the tank is of type PlayerTankAlgorithm
    int worked;    
    
    if(tankRef.getCurrTurn() == lastTurnMapUpdated){
        worked = enemysTanks.empty();

        if (worked)
        {
            return; // Exit if the game board has not been updated for the current turn
        }
        
        worked = playerTanks.empty();
        
        if (worked)
        {
            return; // Exit if the game board has not been updated for the current turn
        }
    }
    array<int, 3> closestEnemy;
    // Update the tank's game board with the satellite view
    calculatePlayerGameBoard(satellite_view, dynamic_cast<PlayerTankAlgorithm&>(tank));
    closestEnemy = findClosestEnemy(dynamic_cast<PlayerTankAlgorithm&>(tank));
    PlayerBattleInfo tankBattleInfo(closestEnemy[0], closestEnemy[1], closestEnemy[2], playerGameBoard, num_shells); // Initialize the battle info with the closest enemy coordinates and turns until next update
    tank.updateBattleInfo(tankBattleInfo); // Update the tank's battle info
}

array<int, 3> OurPlayer::findClosestEnemy(PlayerTankAlgorithm& tank) const {
    int enemyCol = -1;
    int enemyRow = -1;
    int minDistance = INT_MAX;
    array<int, 3> closestEnemy = {-1, -1, -1}; // Initialize closest enemy coordinates and distance
    for(shared_ptr<movingObject> enemy : enemysTanks){
        enemyCol = enemy->getLocation()[0];
        enemyRow = enemy->getLocation()[1];
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
