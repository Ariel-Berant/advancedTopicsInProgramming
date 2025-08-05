#include "PlayerTank.h"

REGISTER_TANK_ALGORITHM(Player2TankAlgorithm);

using namespace Algorithm_0000;

Player2TankAlgorithm::Player2TankAlgorithm(int row, int col, orientation orient)  : PlayerTankAlgorithm(row, col, orient, P2T) {
    static vector<vector<array<shared_ptr<matrixObject>, 2>>> dummyBoard;
    tankBattleInfo = make_unique<PlayerBattleInfo>(-1, -1, -1, dummyBoard, 0);
}


// Function to compute the direction from (col, row) offsets
orientation getDirectionFromOffset(int colOffset, int rowOffset) {
    if (rowOffset == -1 && colOffset == 0) return U;  // U
    if (rowOffset == -1 && colOffset == 1) return UR;  // UR
    if (rowOffset == 0 && colOffset == 1) return R;   // R
    if (rowOffset == 1 && colOffset == 1) return DR;   // DR
    if (rowOffset == 1 && colOffset == 0) return D;   // D
    if (rowOffset == 1 && colOffset == -1) return DL;  // DL
    if (rowOffset == 0 && colOffset == -1) return L;  // L
    if (rowOffset == -1 && colOffset == -1) return UL; // UL
    return UNKNOWN;// Invalid offset
}


int calculateTurnsToRotate(int startOrient, int endOrient) {
    // Calculate the absolute difference
    int diff = abs(endOrient - startOrient);

    // Find the minimum steps (clockwise or counterclockwise)
    int steps = std::min(diff, 8 - diff);

    // Calculate the number of turns (1 or 2 steps per turn)
    int numTurns = static_cast<int>(std::ceil(steps / 2.0));

    return numTurns;
}

int calculateFirstStepInRotate(int startOrient, int endOrient) {
    // Calculate clockwise and counterclockwise distances
    int clockwise_steps = (endOrient - startOrient + 8) % 8;
    int counterclockwise_steps = (startOrient - endOrient + 8) % 8;

    // Determine the first step direction
    if (clockwise_steps <= counterclockwise_steps) {
        return (startOrient + 1) % 8; // Move 1 step clockwise
    } else {
        return (startOrient - 1 + 8) % 8; // Move 1 step counterclockwise
    }
}

// the return value is {x, y, distance, orientation}
array<int,4> Player2TankAlgorithm::searchForBullets(int inCol, int inRow) const{
    int numOfCols = tankBattleInfo->getGameBoard().size();
    int numOfRows = tankBattleInfo->getGameBoard()[0].size();

    for(int i = 1; i<= 8 ; i++){

        int row = (i * numOfRows + location[1] + i * inRow) % numOfRows;
        int col = (i * numOfCols + location[0] + i * inCol) % numOfCols;
        auto& cell = tankBattleInfo->getGameBoard()[col][row][1];
        auto bulletPtr = std::dynamic_pointer_cast<bullet>(cell);
        if(bulletPtr && bulletPtr->getType() == B){
            auto bulletPtr = std::dynamic_pointer_cast<bullet>(cell);
            if(bulletPtr->getOrientation() == UNKNOWN){
                bulletPtr->setOrientation(getDirectionFromOffset(-inCol, -inRow));
            }
            if(bulletPtr->getOrientation() == getDirectionFromOffset(-inCol, -inRow)){
                bulletPtr->setNewLocation(bulletPtr->getLocation()[1], bulletPtr->getLocation()[0]);
                bulletPtr->setNewLocation(bulletPtr->getLocation()[1], bulletPtr->getLocation()[0]);
                return {col, row, i, getDirectionFromOffset(inCol, inRow)};
            }
        }
    }
    return {0,0,0,0};
}

ActionRequest Player2TankAlgorithm::calculateRun(array<int,4> closestBulletDetails, int numOfCols, int numOfRows, int numOfBulletsChasing) {
    int numOfTurnsToRotate = calculateTurnsToRotate(orient, closestBulletDetails[3]);
    if(numOfTurnsToRotate + max(0, turnsUntilNextShot - numOfTurnsToRotate ) + 1 <= closestBulletDetails[2]/2 && numOfBulletsChasing <= 1) {
        // if the tank can shoot the bullet before it gets to him there is only one bullet chasing him
        if(numOfTurnsToRotate == 0 && canShoot()){
            return ActionRequest::Shoot; // if the tank is already facing the bullet, just shoot
        }
        else{
            return determineNextMove(orient, calculateFirstStepInRotate(orient, closestBulletDetails[3])).first;//change the orientation in order to shoot
        }
    }
    else{
        ActionRequest nextMove = findAdjSafe(numOfCols, numOfRows, P2T, closestBulletDetails[2]).first;
        if(nextMove == ActionRequest::DoNothing){
            return ActionRequest::DoNothing;
        }
        else{
            return nextMove;
        }
    }
}

ActionRequest Player2TankAlgorithm::calculateNoDangerAction(const int numOfCols, const int numOfRows){
    const int closestEnemyLoc[2] = {tankBattleInfo->getClosestEnemyTankCol(), tankBattleInfo->getClosestEnemyTankRow()}; 
    int targetOrientation = calculateTargetOrientation(closestEnemyLoc[0], closestEnemyLoc[1]);
    pair<ActionRequest, int> next = determineNextMove(orient, targetOrientation);
    if(checkIfOnSameLine(closestEnemyLoc)){
        if(canShoot()){
            // if the tank can shoot and its already facing the target tank- shoot
            if(friendlyFireRisk(numOfCols, numOfRows, closestEnemyLoc[0], closestEnemyLoc[1])){
                return ActionRequest::GetBattleInfo; // if there is a risk of friendly fire, ask for battle info
            }
            else{
                return ActionRequest::Shoot;
            }
        }
        else if(next.second != 0){
            // the tank doesnt facing the target tank, thus must to rotate - perform the move
            return next.first;
        }
    }

    else{
        unique_ptr<int[]> newLoc = newLocation(numOfCols, numOfRows);
        if(isSafe(newLoc[0], newLoc[1], numOfCols, numOfRows, 1, P2T) && next.first == ActionRequest::MoveForward){
            // we don't need to change the move it's stay next.first
        }
        else{
            if(next.first != ActionRequest::MoveForward && calcMoveRound == 0){
                return next.first; // we need to change the orientation and thus it's stay next.first
            }
            int targetLocInCol[2] = {closestEnemyLoc[0],location[1]}; 
            int targetLocInRow[2] = {location[0], closestEnemyLoc[1]};
            int targetOrientation = calculateTargetOrientation(targetLocInCol[0], targetLocInCol[1]);
            next = determineNextMove(orient, targetOrientation);

            if(isSafe((targetLocInCol[0] + 1) % numOfCols, targetLocInCol[1], numOfCols, numOfRows, 1, P2T) && isSafe((targetLocInCol[0] + 1) % numOfCols, targetLocInCol[1], numOfCols, numOfRows, 2, P2T)){
                return next.first; // if the tank can move forward to the target location, do it
            }
            else if(isSafe(targetLocInCol[0] - 1, targetLocInCol[1], numOfCols, numOfRows, 1, P2T) && isSafe(targetLocInCol[0] - 1, targetLocInCol[1], numOfCols, numOfRows, 2, P2T)){
                next = determineNextMove(orient, calculateTargetOrientation(targetLocInRow[0], targetLocInRow[1]));
            }
            else{
                int targetOrientation = calculateTargetOrientation(targetLocInRow[0], targetLocInRow[1]);
                next = determineNextMove(orient, targetOrientation);
                if(isSafe(targetLocInRow[0], targetLocInRow[1] + 1, numOfCols, numOfRows, 1, P2T) && isSafe(targetLocInRow[0], targetLocInRow[1] + 1, numOfCols, numOfRows, 2, P2T)){
                    next = determineNextMove(orient, targetOrientation);
                }
                else if(isSafe(targetLocInRow[0], targetLocInRow[1] - 1, numOfCols, numOfRows, 1, P2T) && isSafe(targetLocInRow[0], targetLocInRow[1] - 1, numOfCols, numOfRows, 2, P2T)){
                    next = determineNextMove(orient, targetOrientation);
                }
            }
        }

        return next.first;
    }
    return next.first;
}

//the return value is {details of the closest danger -{x, y, distance, orientation} , numOfBulletsChasing}
pair<array<int,4>,int> Player2TankAlgorithm::searchForDangerObjects(){
    int numOfBulletsChasing = 0;
    int closestBulletDist = 9;
    array<int,4> closestBulletDetails ={0};
    for(int dir=0; dir<8; dir++) {
        pair<int, int> offset = getDirectionOffset(orientation(dir));
        array<int, 4> currentBulletDetails = searchForBullets(offset.first, offset.second);
        if(currentBulletDetails[2] >=1){
            numOfBulletsChasing++;
            if(currentBulletDetails[2] < closestBulletDist){
                closestBulletDist = currentBulletDetails[2];
                closestBulletDetails = currentBulletDetails;
            }
        }
    }
    closestBulletDetails[2] = closestBulletDist; // Update the distance in the details
    return {closestBulletDetails, numOfBulletsChasing}; // Return the closest bullet details and the number of bullets chasing
}

ActionRequest Player2TankAlgorithm::noDangerAction(int numOfCols, int numOfRows, const int closestEnemyLoc[2]){
    unique_ptr<int[]> newLoc = newLocation(numOfCols, numOfRows);
    if(tankBattleInfo->getTurnsUntillNextUpdate() + 1 == 0 || currTurn % 4 == 1){
        return ActionRequest::GetBattleInfo; // If it's time to update the battle info, return the request
    }
    else if(checkIfOnSameLine(closestEnemyLoc) ||(tankBattleInfo->getGameBoard()[newLoc[0]][newLoc[1]][1] && tankBattleInfo->getGameBoard()[newLoc[0]][newLoc[1]][1]->getType() == P1T) ){
        if((tankBattleInfo->getGameBoard()[newLoc[0]][newLoc[1]][1] && tankBattleInfo->getGameBoard()[newLoc[0]][newLoc[1]][1]->getType() == P1T)){
            if (canShoot()){
                return ActionRequest::Shoot; // If the tank can shoot, shoot
            }
        }
        else{
            return determineNextMove(orient, calculateTargetOrientation(tankBattleInfo->getClosestEnemyTankCol(), tankBattleInfo->getClosestEnemyTankRow())).first;
        }
    }
    else{
        return  calculateNoDangerAction(numOfCols, numOfRows);
    }

    return ActionRequest::GetBattleInfo; // Default action if something goes wrong
}


ActionRequest Player2TankAlgorithm::searchAndDealWithDanger(int numOfCols, int numOfRows){
    pair<array<int,4>,int> dangerSeekResult = searchForDangerObjects();
    array<int,4> closestBulletDetails = dangerSeekResult.first;
    int closestBulletDist = closestBulletDetails[2];
    int numOfBulletsChasing = dangerSeekResult.second;
    if(closestBulletDist != 9){//if there is a bullet chasing the tank
        return calculateRun(closestBulletDetails, numOfCols, numOfRows, numOfBulletsChasing);
    }
    return ActionRequest::DoNothing;
}


ActionRequest Player2TankAlgorithm::getAction(){
    updateTurn();
    if(currTurn == 1){
        return ActionRequest::GetBattleInfo;
    }
    const int numOfCols = tankBattleInfo->getGameBoard().size(); 
    const int numOfRows = tankBattleInfo->getGameBoard()[0].size();
    moveTankBullets(numOfCols, numOfRows);
    const int closestEnemyLoc[2] = {tankBattleInfo->getClosestEnemyTankCol(), tankBattleInfo->getClosestEnemyTankRow()}; 
    unique_ptr<int[]> newLoc = newLocation(numOfCols, numOfRows);
    if(shotsLeft < 0){ // if the tank still doesn't know with how many bullets it started the game
        shotsLeft = tankBattleInfo->getNumOfStartingTankBullets();
    }
    bool running = false;
    ActionRequest chosenMove = searchAndDealWithDanger(numOfCols, numOfRows);
    if(chosenMove != ActionRequest::DoNothing){
        // this will be the chosen move- safety first
        running = true;
    }
    // if there is no danger
    else{
        chosenMove = noDangerAction(numOfCols, numOfRows, closestEnemyLoc);
    }
    if (chosenMove == ActionRequest::MoveForward && tankBattleInfo->getGameBoard()[newLoc[0]][newLoc[1]][0] && tankBattleInfo->getGameBoard()[newLoc[0]][newLoc[1]][0]->getType() == W && running != true) {
        // If the tank can't move forward because of a wall, shoot the wall
        if(canShoot()){
            chosenMove = ActionRequest::Shoot;
        }
        else{
            chosenMove = ActionRequest::GetBattleInfo; // If the tank can't shoot, do nothing
        }
    }
    tankBattleInfo->setTurnsUntillNextUpdate();
    tankBattleInfo->setTurnsFromLastUpdate();
    updateTankData(chosenMove, numOfCols, numOfRows); // Default action if something goes wrong
    moveTankBullets(numOfCols, numOfRows); // Move the tank's bullets after updating the tank data
    return chosenMove;
}

// Only backwards
// Backwards, then some move(out of all possible moves)
// Backwards into a wall
// Backwards, then forwards, then backwards
// Give all 1 bullet, surround
// 