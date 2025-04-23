#include "movingObject.h"


movingObject::movingObject(int row, int col, objectType oType, orientation orient) : matrixObject(row, col, oType), orient(orient), oldLocation{row, col} {
    // Constructor implementation
    canMove = true; // Initialize canMove to true
}
movingObject::~movingObject(){
    // Destructor implementation
    // No specific cleanup is needed here since the base class destructor will handle it.
    // The destructor is declared virtual in the base class to ensure proper cleanup of derived classes.
}

int *movingObject::newLocation(const int numOfCols, const int numOfRows) const {
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
        newLoc[0] = (numOfRows +  location[0] + 1) % numOfRows;
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
void movingObject::setNewLocation(const int newRow, const int newCol) {
    oldLocation[0] = location[0];
    oldLocation[1] = location[1];
    location[0] = newRow;
    location[1] = newCol;
}
const int* movingObject::getOldLocation() const{
    // Returns a pointer to the location array
    return oldLocation;
}

orientation movingObject::getOrientation() const {
    return orient;
}

