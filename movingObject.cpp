#include "movingObject.h"

movingObject::movingObject(int x, int y, orientation orient) : matrixObject(x, y), orient(orient) {
    // Constructor implementation
    canMove = true; // Initialize canMove to true
}
movingObject::~movingObject() = default;