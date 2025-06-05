#include "PlayerTank.h"

Player2TankAlgorithm::Player2TankAlgorithm(int row, int col, orientation orient)  : PlayerTankAlgorithm(row, col, orient, P2T) {
    static vector<vector<array<shared_ptr<matrixObject>, 2>>> dummyBoard;
    tankBattleInfo = make_unique<PlayerBattleInfo>(-1, -1, -1, dummyBoard, 0);
}

void Player2TankAlgorithm::updateBattleInfo(BattleInfo& info) {
    tankBattleInfo = make_unique<PlayerBattleInfo>(dynamic_cast<PlayerBattleInfo&>(info));
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

        int row = (numOfRows + location[1] + i * inRow) % numOfRows;
        int col = (numOfCols + location[0] + i * inCol) % numOfCols;

        matrixObject* const obj = tankBattleInfo->getGameBoard()[col][row][1].get();
        if(obj && obj->getType() == B){
            bullet *b = dynamic_cast<bullet *>(obj);
            if(b->getOrientation() == UNKNOWN){
                b->setOrientation(getDirectionFromOffset(-inCol, -inRow));
            }
            if(b->getOrientation() == getDirectionFromOffset(-inCol, -inRow)){
                b->setNewLocation(numOfRows, numOfCols);
                b->setNewLocation(numOfRows, numOfCols);
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
    if(next.second == 0 && canShoot()){
        // if the tank can shoot and its already facing the target tank- shoot
        if(friendlyFireRisk(numOfCols, numOfRows)){
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
    return {closestBulletDetails, numOfBulletsChasing}; // Return the closest bullet details and the number of bullets chasing
}



ActionRequest Player2TankAlgorithm::getAction(){
    if(currTurn == 0){
        return ActionRequest::GetBattleInfo;
    }
    updateTurn();
    moveTankBullets(tankBattleInfo->getGameBoard().size(), tankBattleInfo->getGameBoard()[0].size());
    const int numOfCols = tankBattleInfo->getGameBoard().size(); 
    const int numOfRows = tankBattleInfo->getGameBoard()[0].size();
    ActionRequest chosenMove = ActionRequest::DoNothing;
    if(tankBattleInfo->getTurnsFromLastUpdate() == -1){
        return ActionRequest::GetBattleInfo; // If this is the first turn, we need to get the battle info
    }
    if(shotsLeft < 0){ // if the tank still doesn't know with how many bullets it started the game
        shotsLeft = tankBattleInfo->getNumOfStartingTankBullets();
    }
    pair<array<int,4>,int> dangerSeekResult = searchForDangerObjects();
    array<int,4> closestBulletDetails = dangerSeekResult.first;
    int closestBulletDist = closestBulletDetails[2];
    int numOfBulletsChasing = dangerSeekResult.second;
    if(closestBulletDist != 9){//if there is a bullet chasing the tank
        chosenMove = calculateRun(closestBulletDetails, numOfCols, numOfRows, numOfBulletsChasing);
    }
    // if there is no danger
    else if(tankBattleInfo->getTurnsUntillNextUpdate() + 1 == 0 || currTurn % 2 == 0){
        chosenMove = ActionRequest::GetBattleInfo; // If it's time to update the battle info, return the request
    }
    else{
        chosenMove =  calculateNoDangerAction(numOfCols, numOfRows);
    }
    tankBattleInfo->setTurnsUntillNextUpdate();
    tankBattleInfo->setTurnsFromLastUpdate();
    updateTankData(chosenMove, numOfCols, numOfRows); // Default action if something goes wrong
    moveTankBullets(numOfCols, numOfRows); // Move the tank's bullets after updating the tank data
    return chosenMove;
}