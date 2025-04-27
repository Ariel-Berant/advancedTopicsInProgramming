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
    if(calcMoveRound < 7){
        calcMoveRound++;
    } else{
        calcMoveRound = 0;
    }
}


int* tank::newLocationAtReverse(const int numOfCols, const int numOfRows) const {
    int *newLoc = new int[2]{location[0], location[1]};
    if(orient == U) {
        newLoc[0] = (numOfRows + location[0] + 1) % numOfRows;
    }
    else if(orient == UR) {
        newLoc[0] = (numOfRows + location[0] + 1) % numOfRows;
        newLoc[1] = (numOfCols + location[1] - 1) % numOfCols;
    }
    else if(orient == R) {
        newLoc[1] = (numOfCols + location[1] - 1) % numOfCols;
    }
    else if(orient == DR) {
        newLoc[0] = (numOfRows + location[0] - 1) % numOfRows;
        newLoc[1] = (numOfCols + location[1] - 1) % numOfCols;
    }
    else if(orient == D) {
        newLoc[0] = (numOfRows + location[0] - 1) % numOfRows;
    }
    else if(orient == DL) {
        newLoc[0] = (numOfRows +  location[0] - 1) % numOfRows;
        newLoc[1] = (numOfCols + location[1] + 1) % numOfCols;
    }
    else if(orient == L) {
        newLoc[1] = (numOfCols + location[1] + 1) % numOfCols;
    }
    else if(orient == UL) {
        newLoc[0] = (numOfRows + location[0] + 1) % numOfRows;
        newLoc[1] = (numOfCols + location[1] + 1) % numOfCols;
    }
    return newLoc;
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

bool tank::canSeeOtherTank(const int otherLoc[2], const vector<vector<array<matrixObject *, 3>>> &gameBoard, int numOfRows, int numOfCols) const {
    int move[2] = {0, 0}, currLoc[2] = {location[0], location[1]};
    matrixObject* matObj;
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