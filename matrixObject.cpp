#include "matrixObject.h"

matrixObject::matrixObject(int x, int y) : location{x, y}
{
    // Constructor implementation
    // The member initializer list initializes the location array with the provided x and y values.
    // No additional code is needed here since the initialization is done in the initializer list.
}

matrixObject::~matrixObject() = default;

const int* matrixObject::getLocation() const
{
    // Returns a pointer to the location array
    return location;
}

void matrixObject::setLocation(int x, int y)
{
    // Sets the location of the object
    location[0] = x;
    location[1] = y;
}

bool matrixObject::getIsAlive() const {
    return isAlive;
}

bool matrixObject::getCanMove() const {
    return canMove;
}