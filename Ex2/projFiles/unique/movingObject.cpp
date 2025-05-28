#include "MovingObject.h"

movingObject::movingObject(int row, int col, objectType oType, orientation orient) : matrixObject(row, col, oType), orient(orient), oldLocation{row, col} {
    // Constructor implementation
}

movingObject::~movingObject(){
    // Destructor implementation
    // No specific cleanup is needed here since the base class destructor will handle it.
    // The destructor is declared virtual in the base class to ensure proper cleanup of derived classes.
}

unique_ptr<int[]> movingObject::newLocation(int numOfCols, int numOfRows, bool atReverse) const {
    auto newLoc = make_unique<int[]>(2);
    newLoc[0] = location[0];
    newLoc[1] = location[1];
    int i = atReverse == true ? -1 : 1; // Determine the direction of movement based on atReverse
    if(orient == L) {
        newLoc[0] = (numOfCols + location[0] - 1 * i) % numOfCols;
    }
    else if(orient == DL) {
        newLoc[0] = (numOfCols + location[0] - 1 * i) % numOfCols;
        newLoc[1] = (numOfRows + location[1] + 1 * i) % numOfRows;
    }
    else if(orient == D) {
        newLoc[1] = (numOfRows + location[1] + 1 * i) % numOfRows;
    }
    else if(orient == DR) {
        newLoc[0] = (numOfCols + location[0] + 1 * i) % numOfCols;
        newLoc[1] = (numOfRows + location[1] + 1 * i) % numOfRows;
    }
    else if(orient == R) {
        newLoc[0] = (numOfCols + location[0] + 1 * i) % numOfCols;
    }
    else if(orient == UR) {
        newLoc[0] = (numOfCols +  location[0] + 1) * i % numOfCols;
        newLoc[1] = (numOfRows + location[1] - 1 * i) % numOfRows;
    }
    else if(orient == U) {
        newLoc[1] = (numOfRows + location[1] - 1 * i) % numOfRows;
    }
    else if(orient == UL) {
        newLoc[0] = (numOfCols + location[0] - 1 * i) % numOfCols;
        newLoc[1] = (numOfRows + location[1] - 1 * i) % numOfRows;
    }
    return newLoc;
}
void movingObject::setNewLocation(const int newRow, const int newCol) {
    oldLocation[0] = location[0];
    oldLocation[1] = location[1];
    location[1] = newRow;
    location[0] = newCol;
}
const int* movingObject::getOldLocation() const{
    // Returns a pointer to the location array
    return oldLocation;
}

orientation movingObject::getOrientation() const {
    return orient;
}

void movingObject::setOrientation(const orientation newOrient) {
    orient = newOrient;
}
