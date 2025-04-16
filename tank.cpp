#include "tank.h"

tank::tank(int x, int y, orientation orient, objectType oType) : movingObject(x, y,oType , orient), shotsLeft(16), inBackwords(0) {
    // Constructor implementation
    // The member initializer list initializes the base class movingObject with x, y, and orientation.
    // It also initializes the tank_size, shotsLeft, and inBackwords members.
}
tank::~tank() {
    // Destructor implementation
    // No specific cleanup is needed for this class, but the destructor is defined for completeness.
}

void tank::setOrientation(const orientation newOrient) {
    orient = newOrient;
}
bool tank::canShot() {
    return shotsLeft > 0;
}
int tank::newLocationAtReverse(const int numOfCols, const int numOfRows) const {
    int *newLoc = new int[2]{location[0], location[1]};
    if(orient == U) {
        newLoc[0] = (location[0] - 1) % numOfRows;
    }
    else if(orient == UR) {
        newLoc[0] = (location[0] - 1) % numOfRows;
        newLoc[1] = (location[1] + 1) % numOfCols;
    }
    else if(orient == R) {
        newLoc[1] = (location[1] + 1) % numOfCols;
    }
    else if(orient == DR) {
        newLoc[0] = (location[0] + 1) % numOfRows;
        newLoc[1] = (location[1] + 1) % numOfCols;
    }
    else if(orient == D) {
        newLoc[0] = (location[0] + 1) % numOfRows;
    }
    else if(orient == DL) {
        newLoc[0] = (location[0] + 1) % numOfRows;
        newLoc[1] = (location[1] - 1) % numOfCols;
    }
    else if(orient == L) {
        newLoc[1] = (location[1] - 1) % numOfCols;
    }
    else if(orient == UL) {
        newLoc[0] = (location[0] - 1) % numOfRows;
        newLoc[1] = (location[1] - 1) % numOfCols;
    }
    return newLoc;
}