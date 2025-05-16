#include "playerTank.h"

Player2TankAlgorithm::Player2TankAlgorithm(int row, int col, orientation orient)  : PlayerTankAlgorithm(row, col, orient, P2T) {}

ActionRequest Player1TankAlgorithm::getAction() {
    // Implement the logic to get the action for Player 1's tank
    // This is a placeholder implementation
    return ActionRequest();
}

void Player1TankAlgorithm::updateBattleInfo(BattleInfo& info) {
    // Implement the logic to update the battle information for Player 1's tank
    // This is a placeholder implementation
}

// Function to compute the direction from (row, col) offsets
int getDirectionFromOffset(int rowOffset, int colOffset) {
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
array<int,4> Player2TankAlgorithm::searchForBullets(const vector<vector<array<shared_ptr<matrixObject>, 3>>> &gameBoard, int inRow, int inCol) const{
    for(int i = 1; i<= 6 ; i++){
        int row = (gameBoard.size() + location[0] + i * inRow) % gameBoard.size();
        int col = (gameBoard[0].size() + location[1] + i * inCol) % gameBoard[0].size();
        matrixObject* const obj = gameBoard[row][col][1].get();
        if(obj && obj->getType() == B && dynamic_cast<bullet *>(obj)->getOrientation() == getDirectionFromOffset(-inRow, -inCol)){
            return {row, col, i, getDirectionFromOffset(inRow, inCol)};
        }
    }
    return {0,0,0,0};
}


objMove Player2TankAlgorithm::play(const vector<vector<array<shared_ptr<matrixObject>, 3>>> &gameBoard, const int otherLoc[2], const int numOfCols, const int numOfRows){

    int numOfBulletsChasing = 0;
    int closestBulletDist = 7;
    array<int,4> closestLocation ={0};
    for(int dir=0; dir<8; dir++) {
        pair<int, int> offset = getDirectionOffset(dir);
        array<int, 4> curLocation = searchForBullets(gameBoard, offset.first, offset.second);
        if(curLocation[2] >=1){
            numOfBulletsChasing++;
            if(curLocation[2] < closestBulletDist){
                closestBulletDist = curLocation[2];
                closestLocation = curLocation;
            }
        }
    }
    if(closestBulletDist != 7){//if there is a bullet chasing the tank
        calcMoveRound = 0;
        int numOfTurnsToRotate = calculateTurnsToRotate(orient, closestLocation[3]);
        if(numOfTurnsToRotate + max(0, turnsUntilNextShot - numOfTurnsToRotate ) + 1 <= closestBulletDist/2 && numOfBulletsChasing <= 1) {
            // if the tank can shoot the bullet before it gets to him there is only one bullet chasing him
            if(numOfTurnsToRotate == 0){
                return shoot;
            }
            else{
                return determineNextMove(orient, calculateFirstStepInRotate(orient, closestLocation[3])).first;//change the orientation in order to shoot
            }
        }
        else{
            objMove nextMove = findAdjSafe(gameBoard, numOfCols, numOfRows, closestBulletDist).first;
            if(nextMove == noAction){
                return shoot;
            }
            else{
                return nextMove;
            }
        }
    }
    
    else{// if there is no danger
        int targetOrientation = calculateTargetOrientation(otherLoc[0], otherLoc[1]);
        pair<objMove, int> next = determineNextMove(orient, targetOrientation);
        if(next.first == moveForward && canShoot()){
            // if the tank can shoot and in order to get to the other tank he needs to move forward- shoot
            return shoot;
        }
        else if(next.first != moveForward && calcMoveRound == 0){
            // if in order to get to the other tank he doesn't need to move forward- perform the move
            return next.first;
        }
        else{
            unique_ptr<int[]> newLoc = newLocation(numOfCols, numOfRows);
            if(isSafe(newLoc[0], newLoc[1], gameBoard, numOfCols, numOfRows, 1) && next.first == moveForward){
                // we don't need to change the move it's stay next.first
            }
            else{

                if(next.first != moveForward && calcMoveRound == 0){
                    // we don't need to change the move it's stay next.first
                }
                else{
                    next = findAdjSafe(gameBoard, numOfCols, numOfRows);
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
    return noAction;
}