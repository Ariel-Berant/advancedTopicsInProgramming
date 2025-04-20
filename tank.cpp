#include "tank.h"

tank::tank(int x, int y, orientation orient, objectType oType) : movingObject(x, y,oType , orient), shotsLeft(16), inBackwards(0), turnsUntilNextShot(0) {
    // Constructor implementation
    // The member initializer list initializes the base class movingObject with x, y, and orientation.
    // It also initializes the tank_size, shotsLeft, and inBackwards members.
}
tank::~tank() {
    // Destructor implementation
    // No specific cleanup is needed for this class, but the destructor is defined for completeness.
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

// Constant since it doesn't modify the object
orientation tank::getOrientation() const {
    return orient;
}

bool tank::canShoot() {
    return shotsLeft > 0 && turnsUntilNextShot == 0;
}

void tank::useShot() {
    turnsUntilNextShot = 4;
    shotsLeft--;
}

void tank::updateTurnsUntilNextShot() {
    if(turnsUntilNextShot > 0){
        turnsUntilNextShot--;
    }
}

int* tank::newLocationAtReverse(const int numOfCols, const int numOfRows) const {
    int *newLoc = new int[2]{location[0], location[1]};
    if(orient == U) {
        newLoc[0] = (numOfRows + location[0] - 1) % numOfRows;
    }
    else if(orient == UR) {
        newLoc[0] = (numOfRows + location[0] - 1) % numOfRows;
        newLoc[1] = (numOfCols + location[1] + 1) % numOfCols;
    }
    else if(orient == R) {
        newLoc[1] = (numOfCols + location[1] + 1) % numOfCols;
    }
    else if(orient == DR) {
        newLoc[0] = (numOfRows + location[0] + 1) % numOfRows;
        newLoc[1] = (numOfCols + location[1] + 1) % numOfCols;
    }
    else if(orient == D) {
        newLoc[0] = (numOfRows + location[0] + 1) % numOfRows;
    }
    else if(orient == DL) {
        newLoc[0] = (numOfRows + location[0] + 1) % numOfRows;
        newLoc[1] = (numOfCols + location[1] - 1) % numOfCols;
    }
    else if(orient == L) {
        newLoc[1] = (numOfCols + location[1] - 1) % numOfCols;
    }
    else if(orient == UL) {
        newLoc[0] = (numOfRows + location[0] - 1) % numOfRows;
        newLoc[1] = (numOfCols + location[1] - 1) % numOfCols;
    }
    return newLoc;
}
