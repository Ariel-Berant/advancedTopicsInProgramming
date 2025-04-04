#include "movingObject.h"

movingObject::movingObject(int x, int y, int orientation) : matrixObject(x, y), orientation(orientation) {
    // Constructor implementation
    canMove = true; // Initialize canMove to true
}
movingObject::~movingObject() {
    // Destructor implementation
    // No specific cleanup is needed for this class, but the destructor is defined for completeness.
}