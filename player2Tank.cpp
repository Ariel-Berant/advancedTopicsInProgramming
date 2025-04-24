#include "playerTank.h"

p2Tank::p2Tank(int row, int col, orientation orient)  : tank(row, col, orient, P2T) {}

// Function to compute the direction from (row, col) offsets
int getDirectionFromOffset(int row, int col) {
        // Handle the special case where there's no displacement

        // Calculate the angle in radians
        double angle = atan2(-row, col); // Negate row for matrix coordinate system
    
        // Normalize the angle to a direction (0 to 7)
        int direction = static_cast<int>(round(4 * angle / M_PI)) % 8;
    
        // Ensure the direction is positive
        if (direction < 0) {
            direction += 8;
        }
    
        return direction;
}
// Function to get the row and col offsets based on the direction
pair<int, int> getDirectionOffset(int dir) {
    // Calculate the angle in radians
    // Calculate the angle in radians
    double angle = M_PI / 4.0 * (7 - dir);

    // Calculate row and col displacements using sin and cos
    int rowDisplacement = -round(sin(angle)); // Flip for matrix row (vertical axis)
    int colDisplacement = round(cos(angle));  // Horizontal axis
    return {rowDisplacement, colDisplacement};
}

pair<int, int> p2Tank::getNeighborPointGivenOrient(int orient){
    pair<int, int> off = getDirectionOffset(orient);
    off.first += location[0];
    off.second += location[1];
    return off;
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


// Function to calculate the target orientation based on position differences
int calculateTargetOrientation(int startRow, int startCol, int targetRow, int targetCol) {
    // Calculate the offset
    int offsetRow = targetRow - startRow;
    int offsetCol = targetCol - startCol;

    // Handle the edge case where start equals target
    if (offsetRow == 0 && offsetCol == 0) {
        return -1; // No orientation needed
    }

    // Calculate the angle in radians
    double angle = atan2(-offsetRow, offsetCol); // Negate offsetRow for matrix row system

    // Convert the angle to one of the 8 directions
    int direction = static_cast<int>(round(4 * angle / M_PI)) % 8;

    // Ensure the direction is positive
    if (direction < 0) {
        direction += 8;
    }

    return direction;
}

// Function to determine the correct move to reach the target
pair<objMove, int> p2Tank::determineNextMove(int currentOrientation, int targetOrientation) {
  if (targetOrientation == -1) {
        return {noAction,0}; // No valid move if already at the target
    }

    // Calculate the difference in orientation
    int diff = (targetOrientation - currentOrientation + 8) % 8;

    // Determine the next move based on the difference in orientation
    if (diff == 0) {
        return {moveForward, 1}; // Already facing the target
    } else if (diff == 1) {
        return {rotateEighthRight,1}; // 1 step clockwise
    } else if (diff == 2) {
        return {rotateQuarterRight, 1}; // 2 steps clockwise
    } else if (diff == 3) {
        return {rotateQuarterRight, 2}; // 3 steps clockwise
    } else if (diff == 4) {
        return {rotateQuarterRight, 2}; // 4 steps clockwise (180-degree turn)
    } else if (diff == 5) {
        return {rotateQuarterLeft, 2}; // 3 steps counterclockwise (5/8ths clockwise equivalent)
    } else if (diff == 6) {
        return {rotateQuarterLeft, 1}; // 2 steps counterclockwise
    } else if (diff == 7) {
        return {rotateEighthLeft, 1}; // 1 step counterclockwise
    }

    return {noAction,0}; // Default action if something goes wrong
}


// the return value is {x, y, distance, orientation}
array<int,4> p2Tank::searchForBullets(const vector<vector<array<matrixObject *, 3>>> &gameBoard, int inRow, int inCol){
    for(int i = 1; i<= 6 ; i++){
        int row = (location[0] + i * inRow) % gameBoard.size();
        int col = (location[1] + i * inCol) % gameBoard[0].size();
        matrixObject* obj = gameBoard[row][col][1];
        if(obj && obj->getType() == B && dynamic_cast<bullet *>(obj)->getOrientation() == getDirectionFromOffset(-inRow, -inCol)){
            return {row, col, i, getDirectionFromOffset(inRow, inCol)};
        }
    }
    return {0,0,0,0};
}
objMove p2Tank::play(const vector<vector<array<matrixObject *, 3>>> &gameBoard, const int otherLoc[2], const int numOfCols, const int numOfRows){
    int numOfBulletsChasing = 0;
    int closestBulletDist = 7;
    array<int,4> closestLocation ={0};
    for(int dir=0; dir<8; dir++) {
        pair<int, int> offset = getDirectionOffset(dir);
        array<int, 4> curLocation = searchForBullets(gameBoard, offset.first, offset.second);
        if(curLocation[2] >=1){
            numOfBulletsChasing++;
        }
        if(curLocation[2] < closestBulletDist){
            closestBulletDist = curLocation[2];
            closestLocation = curLocation;
        }
    }
    if(closestBulletDist != 7){//if there is a bullet chasing the tank
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
            int targetOrientation;
            for(int orien = 0 ;orien < 8 ; orien++){//search for a safe place from the bullets
                pair<int,int> pointToCheck = getNeighborPointGivenOrient(orien);
                targetOrientation = calculateTargetOrientation(location[0], location[1], pointToCheck.first, pointToCheck.second);
                pair<objMove, int> val =  determineNextMove(orien, targetOrientation);
                objMove nextMove = val.first;
                int numOfMoves = val.second;
                if(isSafe(pointToCheck.first, pointToCheck.second, gameBoard, numOfCols, numOfRows, numOfMoves)
                        && isSafe(pointToCheck.first, pointToCheck.second, gameBoard, numOfCols, numOfRows, numOfMoves + 1)){
                    if(numOfMoves <= (closestBulletDist/2)){
                        return nextMove;
                    }
                }
            }
            return shoot;
        }
    }
    else{// if there is no danger
          if(canSeeOtherTank(otherLoc, gameBoard, numOfCols, numOfRows)){
              return shoot;
          }
          else{
              int targetOrientation = calculateTargetOrientation(location[0], location[1], otherLoc[0], otherLoc[1]);
              return determineNextMove(orient, targetOrientation).first;
          }

    }
}