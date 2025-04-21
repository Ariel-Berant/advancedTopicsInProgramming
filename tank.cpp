#include "tank.h"

tank::tank(int x, int y, orientation orient, objectType oType) : movingObject(x, y,oType , orient), shotsLeft(16), inBackwards(0), turnsUntilNextShot(0), calcMoveRound(0) {
    // Constructor implementation
    // The member initializer list initializes the base class movingObject with x, y, and orientation.
    // It also initializes the tank_size, shotsLeft, and inBackwards members.
}
tank::~tank() = default;

int tank::getInBack() const {
    return inBackwards;
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

void tank::useShot() {
    turnsUntilNextShot = 4;
    shotsLeft--;
}

void tank::updateTurn() {
    if(turnsUntilNextShot > 0){
        turnsUntilNextShot--;
    }
    if(calcMoveRound < 7){
        calcMoveRound++;
    } else{
        calcMoveRound = 0;
    }
}


int* tank::newLocationAtReverse(const int numOfCols, const int numOfRows) const {
    int *newLoc = new int[2]{location[0], location[1]};
    if(orient == U) {
        newLoc[1] = (numOfRows + location[1] + 1) % numOfRows;
    }
    else if(orient == UR) {
        newLoc[1] = (numOfRows + location[1] + 1) % numOfRows;
        newLoc[0] = (numOfCols + location[0] - 1) % numOfCols;
    }
    else if(orient == R) {
        newLoc[0] = (numOfCols + location[0] - 1) % numOfCols;
    }
    else if(orient == DR) {
        newLoc[1] = (numOfRows + location[1] - 1) % numOfRows;
        newLoc[0] = (numOfCols + location[0] - 1) % numOfCols;
    }
    else if(orient == D) {
        newLoc[1] = (numOfRows + location[1] - 1) % numOfRows;
    }
    else if(orient == DL) {
        newLoc[1] = (numOfRows +  location[1] - 1) % numOfRows;
        newLoc[0] = (numOfCols + location[0] + 1) % numOfCols;
    }
    else if(orient == L) {
        newLoc[0] = (numOfCols + location[0] + 1) % numOfCols;
    }
    else if(orient == UL) {
        newLoc[1] = (numOfRows + location[1] + 1) % numOfRows;
        newLoc[0] = (numOfCols + location[0] + 1) % numOfCols;
    }
    return newLoc;
}

bool tank::isSafe(const int x, const int y, const vector<vector<array<matrixObject*, 3>>>& gameBoard,
                  const int numOfCols, const int numOfRows, const int movesAhead) const{
    matrixObject* unmovingObj = gameBoard[x][y][0];
    matrixObject* movingObj = gameBoard[x][y][1];
    matrixObject* bulletObj;

    // Check for walls or mines
    if (unmovingObj && (unmovingObj->getType() == W || unmovingObj->getType() == M)) {
        return false;
    }

    // Check for bullets
    bool bulletFound = false;

    vector<array<int, 3>> possibleLocs = {
        {(numOfCols + x - 2 * movesAhead) % numOfCols,       y,                                                R},
        {(numOfCols + x + 2 * movesAhead) % numOfCols,       y,                                                L},
        {x,                                                  (numOfRows + y - 2 * movesAhead) % numOfRows,     D},
        {x,                                                  (numOfRows + y + 2 * movesAhead) % numOfRows,     U},
        {(numOfCols + x - 2 * movesAhead) % numOfCols,       (numOfRows + y - 2 * movesAhead) % numOfRows,     DR},
        {(numOfCols + x - 2 * movesAhead) % numOfCols,       (numOfRows + y + 2 * movesAhead) % numOfRows,     UR},
        {(numOfCols + x + 2 * movesAhead) % numOfCols,       (numOfRows + y - 2 * movesAhead) % numOfRows,     DL},
        {(numOfCols + x + 2 * movesAhead) % numOfCols,       (numOfRows + y + 2 * movesAhead) % numOfRows,     UL},
        {(numOfCols + x - 2 * movesAhead - 1) % numOfCols,   y,                                                R},
        {(numOfCols + x + 2 * movesAhead - 1) % numOfCols,   y,                                                L},
        {x,                                                  (numOfRows + y - 2 * movesAhead - 1) % numOfRows, D},
        {x,                                                  (numOfRows + y + 2 * movesAhead - 1) % numOfRows, U},
        {(numOfCols + x - 2 * movesAhead - 1) % numOfCols,   (numOfRows + y - 2 * movesAhead - 1) % numOfRows, DR},
        {(numOfCols + x - 2 * movesAhead - 1) % numOfCols,   (numOfRows + y + 2 * movesAhead - 1) % numOfRows, UR},
        {(numOfCols + x + 2 * movesAhead - 1) % numOfCols,   (numOfRows + y - 2 * movesAhead - 1) % numOfRows, DL},
        {(numOfCols + x + 2 * movesAhead - 1) % numOfCols,   (numOfRows + y + 2 * movesAhead - 1) % numOfRows, UL}
    };


    for (array<int, 3> loc: possibleLocs) {
        bulletObj = gameBoard[loc[0]][loc[1]][1];
        if (bulletObj && dynamic_cast<bullet *>(bulletObj) && dynamic_cast<bullet *>(bulletObj)->getOrientation() == loc[2]) {
            bulletFound = true;
        }
    }

    return bulletFound;
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
        case 1:
            rotations.push_back(rotateEighthLeft);
            break;
        case 6:
            rotations.push_back(rotateQuarterRight);
            break;
        case 5:
            rotations.push_back(rotateQuarterRight);
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

bool tank::canSeeOtherTank(const int otherLoc[2], const vector<vector<array<matrixObject *, 3>>> &gameBoard, int numOfRows, int numOfCols) const {
    int move[2] = {0, 0}, currLoc[2] = {location[0], location[1]};
    matrixObject* matObj;
    bool canSee = false;
    switch (orient){
        case UR:
            move[0] = 1;
        case U:
            move[1] = -1;
            break;
        case DL:
            move[0] = -1;
        case D:
            move[1] = 1;
            break;
        case UL:
            move[1] = -1;
        case L:
            move[0] = -1;
            break;
        case DR:
            move[1] = 1;
        case R:
            move[0] = 1;
            break;
        default:
            break;
    }

    do {
        currLoc[0] = (currLoc[0] + move[0] + numOfCols) % numOfCols;
        currLoc[1] = (currLoc[1] + move[1] + numOfRows) % numOfRows;
        matObj = gameBoard[currLoc[0]][currLoc[1]][0];
        if (matObj && matObj->getType() == W) {
            canSee = false; // Wall encountered
            break;
        }
        if (currLoc[0] == otherLoc[0] && currLoc[1] == otherLoc[1]) {
            canSee = true; // Other tank found
            break;
        }
    } while (currLoc[0] != location[0] && currLoc[1] != location[1]);
    return canSee;
}