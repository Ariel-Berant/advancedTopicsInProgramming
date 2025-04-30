#include "tank.h"

tank::tank(int row, int col, orientation orient, objectType oType) : movingObject(row, col,oType , orient), shotsLeft(16), inBackwards(0), turnsUntilNextShot(0), calcMoveRound(0) {
    // Constructor implementation
    // The member initializer list initializes the base class movingObject with x, y, and orientation.
    // It also initializes the tank_size, shotsLeft, and inBackwards members.
}
tank::~tank(){
    // Destructor implementation
    // No specific cleanup is needed here since the base class destructor will handle it.
    // The destructor is declared virtual in the base class to ensure proper cleanup of derived classes.
}

int tank::getInBack() const {
    return inBackwards;
}

int tank::getNumOfShotsLeft() const {
    return shotsLeft;
}

void tank::setInBackwards(int inBack) {
    inBackwards = inBack;
}

void tank::setOrientation(const orientation newOrient) {
    orient = newOrient;
}

bool tank::canShoot() const {
    return shotsLeft > 0 && turnsUntilNextShot == 0;
}

bool tank::hasBullets() const {
    return shotsLeft > 0;
}

void tank::useShot() {
    turnsUntilNextShot = 4;
    shotsLeft--;
}

void tank::updateTurn() {
    if(turnsUntilNextShot > 0){
        turnsUntilNextShot--;
    }
}

bool tank::isSafe(const int row, const int col, const vector<vector<array<matrixObject*, 3>>>& gameBoard,
                  const int numOfCols, const int numOfRows, const int movesAhead) const{
    matrixObject* unmovingObj = gameBoard[row][col][0];
    matrixObject* bulletObj;

    // Check for walls or mines
    if (unmovingObj && (unmovingObj->getType() == W || unmovingObj->getType() == M)) {
        return false;
    }

    // Check for bullets
    bool bulletNotFound = true;

    vector<array<int, 3>> possibleLocs = {
        {row,                                                  					(numOfCols + ((col - 2 * movesAhead) % numOfCols)) % numOfCols,          	L},
        {row,                                                  					(numOfCols + col + 2 * movesAhead) % numOfCols,          	R},
        {(numOfRows + ((row - 2 * movesAhead) % numOfRows)) % numOfRows,    	col,                                                     	U},
        {(numOfRows + row + 2 * movesAhead) % numOfRows,       					col,                                                     	D},
        {(numOfRows + ((row - 2 * movesAhead) % numOfRows)) % numOfRows,    	(numOfCols + ((col - 2 * movesAhead) % numOfCols)) % numOfCols,          	UL},
        {(numOfRows + row + 2 * movesAhead) % numOfRows,       					(numOfCols + ((col - 2 * movesAhead) % numOfCols)) % numOfCols,          	DL},
        {(numOfRows + ((row - 2 * movesAhead) % numOfRows)) % numOfRows,    	(numOfCols + col + 2 * movesAhead) % numOfCols,          	UR},
        {(numOfRows + row + 2 * movesAhead) % numOfRows,       					(numOfCols + col + 2 * movesAhead) % numOfCols,         	DR},
        {row,                                                  					(numOfCols + ((col - 2 * movesAhead - 1) % numOfCols)) % numOfCols,      	L},
        {row,                                                 					(numOfCols + ((col - 2 * movesAhead - 1) % numOfCols)) % numOfCols,      	R},
        {(numOfRows + ((row - 2 * movesAhead - 1) % numOfRows)) % numOfRows,	col,                                                   		U},
        {(numOfRows + ((row - 2 * movesAhead - 1) % numOfRows)) % numOfRows,  	col,                                                  		   			D},
        {(numOfRows + ((row - 2 * movesAhead - 1) % numOfRows)) % numOfRows,  	(numOfCols + ((col - 2 * movesAhead - 1) % numOfCols)) % numOfCols,      	UL},
        {(numOfRows + ((row - 2 * movesAhead - 1) % numOfRows)) % numOfRows,  	(numOfCols + ((col - 2 * movesAhead - 1) % numOfCols)) % numOfCols,    	DL},
        {(numOfRows + ((row - 2 * movesAhead - 1) % numOfRows)) % numOfRows,  	(numOfCols + ((col - 2 * movesAhead - 1) % numOfCols)) % numOfCols,    	UR},
        {(numOfRows + ((row - 2 * movesAhead - 1) % numOfRows)) % numOfRows,  	(numOfCols + ((col - 2 * movesAhead - 1) % numOfCols)) % numOfCols,    	DR}
    };


    for (array<int, 3> loc: possibleLocs) {
        bulletObj = gameBoard[loc[0]][loc[1]][1];
        if (bulletObj && dynamic_cast<bullet *>(bulletObj) && dynamic_cast<bullet *>(bulletObj)->getOrientation() == loc[2]) {
            bulletNotFound = false;
        }
    }

    return bulletNotFound;
}


// Function to get the rotations needed to reach the desired orientation from the current orientation
vector<objMove> tank::getRotations(orientation curr, orientation desired) const{
    vector<objMove> rotations = vector<objMove>(0);
    int diff = (8 + curr - desired) % 8;
    switch (diff) {
        case 2:
            rotations.push_back(rotateQuarterLeft);
            break;
        case 3:
            rotations.push_back(rotateQuarterLeft);
            rotations.push_back(rotateEighthLeft);
            break;
        case 1:
            rotations.push_back(rotateEighthLeft);
            break;
        case 6:
            rotations.push_back(rotateQuarterRight);
            break;
        case 5:
            rotations.push_back(rotateQuarterRight);
            rotations.push_back(rotateEighthRight);
            break;
        case 7:
            rotations.push_back(rotateEighthRight);
            break;
        case 4:
            rotations.push_back(rotateQuarterLeft);
            rotations.push_back(rotateQuarterLeft);
            break;
        default:
            break;
    }
    rotations.push_back(moveForward);
    return rotations;
}

bool tank::canSeeOtherTank(const int otherLoc[2], int numOfRows, int numOfCols) const {
    int move[2] = {0, 0}, currLoc[2] = {location[0], location[1]};
    // matrixObject* matObj;
    bool canSee = false;
    switch (orient){
        case UR:
            move[1] = 1;
            move[0] = -1;
            break;
        case U:
            move[0] = -1;
            break;
        case DL:
            move[1] = -1;
            move[0] = 1;
            break;
        case D:
            move[0] = 1;
            break;
        case UL:
            move[1] = -1;
            move[0] = -1;
            break;
        case L:
            move[1] = -1;
            break;
        case DR:
            move[1] = 1;
            move[0] = 1;
            break;
        case R:
            move[1] = 1;
            break;
        default:
            break;
    }

    do {
        currLoc[0] = (currLoc[0] + move[0] + numOfRows) % numOfRows;
        currLoc[1] = (currLoc[1] + move[1] + numOfCols) % numOfCols;
        // matObj = gameBoard[currLoc[0]][currLoc[1]][0];
        // if (matObj && matObj->getType() == W) {
        //     canSee = false; // Wall encountered
        //     break;
        // }
        if (currLoc[0] == otherLoc[0] && currLoc[1] == otherLoc[1]) {
            canSee = true; // Other tank found
            break;
        }
    } while (currLoc[0] != location[0] || currLoc[1] != location[1]);
    return canSee;
}


// Function to check if the tank is surrounded by walls, bullets or mines(if by tank, it will try to shoot it, not surrounded)
bool tank::isSurrounded(const vector<vector<array<matrixObject *, 3>>> &gameBoard, const int *tankLoc) const {
    // Check if the tank is surrounded by walls or bullets
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue; // Skip the tank's own position
            int newRow = (tankLoc[0] + i + gameBoard.size()) % gameBoard.size();
            int newCol = (tankLoc[1] + j + gameBoard[0].size()) % gameBoard[0].size();
            if (gameBoard[newRow][newCol][0] && gameBoard[newRow][newCol][0]->getType() != W) {
                // No wall found
                if (gameBoard[newRow][newCol][1] && gameBoard[newRow][newCol][1]->getType() != B) {
                    // No bullet found
                    if (gameBoard[newRow][newCol][1] && gameBoard[newRow][newCol][1]->getType() != M) {
                        // No mine found - free space
                        return false; 
                    }
                }
            }
        }
    }
    return true; // Surrounded by walls, bullets or mines
}


// Function to calculate the target orientation based on position differences
int tank::calculateTargetOrientation(int targetRow, int targetCol) {
    // Calculate the offset
    int offsetRow = targetRow - location[0];
    int offsetCol = targetCol - location[1];

    // Handle the edge case where start equals target
    if (offsetRow > 0 && offsetCol > 0) {
        return DR;
    }
    else if (offsetRow < 0 && offsetCol < 0) {
        return UL;
    }
    else if (offsetRow == 0) {
        return (offsetCol > 0) ? R : L; // Horizontal movement
    }
    else if (offsetCol == 0) {
        return (offsetRow > 0) ? D : U; // Vertical movement
    }
    else if (offsetRow > 0 && offsetCol < 0) {
        return DL; // Down-Left
    }
    else if (offsetRow < 0 && offsetCol > 0) {
        return UR; // Up-Right
    }
    return 0; // Default case (should not happen)
}

// Function to determine the correct move to reach the target
pair<objMove, int> tank::determineNextMove(int currentOrientation, int targetOrientation) {
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


  pair<objMove, int> tank::findAdjSafe(const vector<vector<array<matrixObject *, 3>>> &gameBoard, int numOfCols, int numOfRows, int closestBulletDist){
    //search for a safest place among all the neighbors cells and return the fist move needed to get there 
    // int targetOrientation;
    for(int orien = 0 ;orien < 8 ; orien++){
        pair<int,int> pointToCheck = getNeighborPointGivenOrient(orien, numOfRows, numOfCols);
        // targetOrientation = calculateTargetOrientation(location[0], location[1], pointToCheck.first, pointToCheck.second);
        pair<objMove, int> movesPair =  determineNextMove(orient, orien);
        int numOfMoves = movesPair.second;
        if(isSafe(pointToCheck.first, pointToCheck.second, gameBoard, numOfCols, numOfRows, numOfMoves)
                && isSafe(pointToCheck.first, pointToCheck.second, gameBoard, numOfCols, numOfRows, numOfMoves + 1)){
            if(numOfMoves <= (closestBulletDist/2)){
                return movesPair;
            }
        }
    }
    return {noAction,0};
}


// Function to get the row and col offsets based on the direction
pair<int, int> tank::getDirectionOffset(int dir) {
    switch (dir) {
        case 0: return {-1, 0};  // U
        case 1: return {-1, 1};  // UR
        case 2: return {0, 1};   // R
        case 3: return {1, 1};   // DR
        case 4: return {1, 0};   // D
        case 5: return {1, -1};  // DL
        case 6: return {0, -1};  // L
        case 7: return {-1, -1}; // UL
        default: return {0, 0};  // Default case (invalid direction)
    }
}

pair<int, int> tank::getNeighborPointGivenOrient(int orient, int numOfROws, int numOfCols) {
    pair<int, int> off = getDirectionOffset(orient);
    off.first = (off.first + location[0] + numOfROws) % numOfROws;
    off.second = (off.second + location[1] + numOfCols) % numOfCols;
    return off;
}