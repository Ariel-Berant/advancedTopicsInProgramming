#include "playerTank.h"

Player2TankAlgorithm::Player2TankAlgorithm(int row, int col, orientation orient)  : PlayerTankAlgorithm(row, col, orient, P2T) {
    tankBattleInfo = make_unique<PlayerBattleInfo>(-1, -1, -1, nullptr);
}

ActionRequest Player2TankAlgorithm::getAction() {
    // Implement the logic to get the action for Player 1's tank
    // This is a placeholder implementation
    return ActionRequest();
}

void Player2TankAlgorithm::updateBattleInfo(BattleInfo& info) {
    // Implement the logic to update the battle information for Player 1's tank
    // This is a placeholder implementation
}

// Function to compute the direction from (col, row) offsets
int getDirectionFromOffset(int colOffset, int rowOffset) {
    if (rowOffset == -1 && colOffset == 0) return 0;  // U
    if (rowOffset == -1 && colOffset == 1) return 1;  // UR
    if (rowOffset == 0 && colOffset == 1) return 2;   // R
    if (rowOffset == 1 && colOffset == 1) return 3;   // DR
    if (rowOffset == 1 && colOffset == 0) return 4;   // D
    if (rowOffset == 1 && colOffset == -1) return 5;  // DL
    if (rowOffset == 0 && colOffset == -1) return 6;  // L
    if (rowOffset == -1 && colOffset == -1) return 7; // UL
    return -1; // Invalid offset
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

        int row = (numOfRows + location[1] + i * inRow) % numOfRows;
        int col = (numOfCols + location[0] + i * inCol) % numOfCols;

        matrixObject* const obj = tankBattleInfo->getGameBoard()[col][row][1].get();

        if(obj && obj->getType() == B && (dynamic_cast<bullet *>(obj)->getOrientation() == getDirectionFromOffset(-inCol, -inRow) || dynamic_cast<bullet *>(obj)->getOrientation() == UNKNOWN)){
            return {col, row, i, getDirectionFromOffset(inCol, inRow)};
        }
    }
    return {0,0,0,0};
}

ActionRequest Player2TankAlgorithm::calculateRun(array<int,4> closestBulletDetails, int numOfCols, int numOfRows, int numOfBulletsChasing) {
    int calcMoveRound = 0;
    int numOfTurnsToRotate = calculateTurnsToRotate(orient, closestBulletDetails[3]);
    if(numOfTurnsToRotate + max(0, turnsUntilNextShot - numOfTurnsToRotate ) + 1 <= closestBulletDetails[2]/2 && numOfBulletsChasing <= 1) {
        // if the tank can shoot the bullet before it gets to him there is only one bullet chasing him
        if(numOfTurnsToRotate == 0){
            return ActionRequest::Shoot; // if the tank is already facing the bullet, just shoot
        }
        else{
            return determineNextMove(orient, calculateFirstStepInRotate(orient, closestBulletDetails[3])).first;//change the orientation in order to shoot
        }
    }
    else{
        ActionRequest nextMove = findAdjSafe(numOfCols, numOfRows, closestBulletDetails[2]).first;
        if(nextMove == ActionRequest::DoNothing){
            return ActionRequest::Shoot;
        }
        else{
            return nextMove;
        }
    }
}

ActionRequest Player2TankAlgorithm::calculateNoDangerAction(const int numOfCols, const int numOfRows){
    int targetOrientation = calculateTargetOrientation(tankBattleInfo->getClosestEnemyTankCol(), tankBattleInfo->getClosestEnemyTankRow());
    pair<ActionRequest, int> next = determineNextMove(orient, targetOrientation);


    if(next.first == ActionRequest::MoveForward && canShoot()){
        // if the tank can shoot and in order to get to the other tank he needs to move forward- shoot
        return ActionRequest::Shoot;
    }

    else if(next.first != ActionRequest::MoveForward && calcMoveRound == 0){
        // if in order to get to the other tank he doesn't need to move forward- perform the move
        return next.first;
    }

    else{
        unique_ptr<int[]> newLoc = newLocation(numOfCols, numOfRows);

        if(isSafe(newLoc[0], newLoc[1], numOfCols, numOfRows, 1) && next.first == ActionRequest::MoveForward){
            // we don't need to change the move it's stay next.first
        }
        else{

            if(next.first != ActionRequest::MoveForward && calcMoveRound == 0){
                // we don't need to change the move it's stay next.first
            }

            else{
                next = findAdjSafe(numOfCols, numOfRows);
                if (calcMoveRound == 0) {
                    calcMoveRound = next.second;
                } 
                else {
                    calcMoveRound--;
                }
            }
        }
        return next.first;
    }
}

ActionRequest Player2TankAlgorithm::getAction(){
    currTurn++;
    int numOfBulletsChasing = 0;
    int closestBulletDist = 9;
    array<int,4> closestLocation ={0};
    if(tankBattleInfo->getTurnsFromLastUpdate() == -1){
        return ActionRequest::GetBattleInfo; // If this is the first turn, we need to get the battle info
    }
    if(shotsLeft < 0){ // if the tank still din't know with how many bullets it started the game
        shotsLeft = tankBattleInfo->getNumOfStartingTankBullets();
    }
    const int numOfCols = tankBattleInfo->getGameBoard().size(); 
    const int numOfRows = tankBattleInfo->getGameBoard()[0].size();

    for(int dir=0; dir<8; dir++) {
        pair<int, int> offset = getDirectionOffset(dir);
        array<int, 4> closestBulletDetails = searchForBullets(offset.first, offset.second);

        if(closestBulletDetails[2] >=1){
            numOfBulletsChasing++;
            if(closestBulletDetails[2] < closestBulletDist){
                closestBulletDist = closestBulletDetails[2];
                closestLocation = closestBulletDetails;
            }
        }
    }
    if(tankBattleInfo->getTurnsUntillNextUpdate() + 1 == 0 || currTurn % 2 == 0){
        return ActionRequest::GetBattleInfo; // If it's time to update the battle info, return the request
    }
    tankBattleInfo->setTurnsUntillNextUpdate();
    tankBattleInfo->setTurnsFromLastUpdate();

    if(closestBulletDist != 9){//if there is a bullet chasing the tank
        return calculateRun(closestLocation, numOfCols, numOfRows, numOfBulletsChasing);
    }
    else{// if there is no danger
        return calculateNoDangerAction(numOfCols, numOfRows);
    }
    return ActionRequest::DoNothing; // Default action if something goes wrong
}