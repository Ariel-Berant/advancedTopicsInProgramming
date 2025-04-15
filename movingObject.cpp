#include "movingObject.h"


movingObject::movingObject(int x, int y, orientation orient) : matrixObject(x, y), orient(orient) {
    // Constructor implementation
    canMove = true; // Initialize canMove to true
}
movingObject::~movingObject() = default;

int *movingObject::newLocation(const int numOfCols, const int numOfRows) const {
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
void movingObject::setLocation(const int newX, const int newY) {
    location[0] = newX;
    location[1] = newY;
}

