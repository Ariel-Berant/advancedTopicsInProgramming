#include "playerTank.h"

p2Tank::p2Tank(int x, int y, orientation orient)  : tank(x, y, orient, P2T) {}

// Function to compute the direction from (x, y) offsets
int getDirectionFromOffset(int x, int y) {
    double angle = atan2(y, x); // Compute angle in radians
    int dir = static_cast<int>(round(angle / (M_PI / 4))) % 8; // Map to direction
    if (dir < 0) dir += 8; // Ensure direction is non-negative
    return dir;
}
// Function to get the x and y offsets based on the direction
pair<int, int> getDirectionOffset(int dir) {
    // Calculate x and y offsets using cos and sin
    int x = round(cos(dir * M_PI / 4)); // M_PI is π
    int y = round(sin(dir * M_PI / 4));
    return {x, y};
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
objMove determineMove(int startOrient, int targetOrient) {
    // Calculate the difference
    int diff = (targetOrient - startOrient + 8) % 8; // Ensure positive difference

    // Map the difference to the appropriate move
    if (diff == 0) {
        return moveForward; // Already facing the target
    } else if (diff == 1) {
        return rotateEighthRight; // 1 step clockwise
    } else if (diff == 2) {
        return rotateQuarterRight; // 2 steps clockwise
    } else if (diff == 3) {
        return rotateQuarterRight; // 3 steps clockwise
    } else if (diff == 4) {
        return rotateQuarterRight; // 4 steps clockwise (180-degree turn)
    } else if (diff == 5) {
        return rotateQuarterLeft; // 3 steps counterclockwise (5/8ths clockwise equivalent)
    } else if (diff == 6) {
        return rotateQuarterLeft; // 2 steps counterclockwise
    } else if (diff == 7) {
        return rotateEighthLeft; // 1 step counterclockwise
    }
    else{
      return noAction;
    }
}

// Function to calculate the target orientation based on position differences
int calculateTargetOrientation(int currentX, int currentY, int targetX, int targetY) {
    // Calculate the direction vector
    int dx = targetX - currentX;
    int dy = targetY - currentY;

    // Handle the edge case where the points are the same
    if (dx == 0 && dy == 0) {
        return -1; // No valid orientation, already at the target
    }

    // Calculate the angle of the direction vector in radians
    double angle = atan2(-dy, dx); // Negative Y is "Up"

    // Normalize the angle to one of the 8 orientations
    int orientation = static_cast<int>(round((angle + M_PI) / (M_PI / 4))) % 8;

    return orientation;
}

// Function to determine the correct move to reach the target
pair<objMove, int> p2Tank::determineNextMove(int currentOrientation, int targetOrientation, int targetX, int targetY) {
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
array<int,4> p2Tank::searchForBullets(const vector<vector<array<matrixObject *, 3>>> &gameBoard, const int numOfCols, const int numOfRows, int inX, int inY){
    for(int i = 1; i<= 6 ; i++){
        matrixObject* obj = gameBoard[location[0] + i * inX][location[1] + i * inY][1];
        if(obj && obj->getType() == B && dynamic_cast<bullet *>(obj)->getOrientation() == getDirectionFromOffset(-inX, -inY)){
            return {location[0] + i * inX, location[1] + i * inY, i, getDirectionFromOffset(inX, inY)};
        }
    }
    return {0,0,0,0};
}
objMove p2Tank::play(const vector<vector<array<matrixObject *, 3>>> &gameBoard, const int otherLoc[2], const int numOfCols, const int numOfRows){
    bool found = false;
    int numOfBulletsChasing = 0;
    int closestBulletDist = 7;
    array<int,4> closestLocation ={0};
    for(int dir=0; dir<8; dir++) {
        pair<int, int> offset = getDirectionOffset(dir);
        array<int, 4> curLocation = searchForBullets(gameBoard, numOfCols, numOfRows, offset.first, offset.second);
        if(curLocation[2] >=1){
            numOfBulletsChasing++;
        }
        if(curLocation[2] < closestBulletDist){
            closestBulletDist = curLocation[2];
            closestLocation = curLocation;
            found = true;
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
                return determineMove(orient, calculateFirstStepInRotate(orient, closestLocation[3]));//change the orientation in order to shoot
            }
        }
        else{
            int targetOrientation;
            for(int orien = 0 ;orien < 8 ; orien++){//search for a safe place from the bullets
                pair<int,int> pointToCheck = getNeighborPointGivenOrient(orien);
                int dx = pointToCheck.first - location[0];
                int dy = pointToCheck.second - location[1];
                targetOrientation = calculateTargetOrientation(location[0], location[1], pointToCheck.first, pointToCheck.second);
                pair<objMove, int> val =  determineNextMove(orien, targetOrientation, pointToCheck.first, pointToCheck.second);
                objMove nextMove = val.first;
                int numOfMoves = val.second;
                if(isSafe(pointToCheck.first, pointToCheck.second, gameBoard, numOfCols, numOfRows, numOfMoves)){
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
              return determineMove(orient, targetOrientation);
          }

    }
}