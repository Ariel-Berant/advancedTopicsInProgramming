#include "MatrixObject.h"

matrixObject::matrixObject() : location{0, 0}
{
    // Default constructor initializes location to (0, 0)
    // The member initializer list initializes the location array with default values.
    // No additional code is needed here since the initialization is done in the initializer list.
}

matrixObject::matrixObject(int row, int col, objectType oType) : location{col, row}, hitsLeft(1), oType(oType)
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

void matrixObject::setLocation(int col, int row)
{
    // Sets the location of the object
    location[0] = col;
    location[1] = row;
}

bool matrixObject::getIsAlive() const {
    return hitsLeft > 0;
}
void matrixObject::takeAHit() {
    hitsLeft--;
}
objectType matrixObject::getType() const {
    return oType;
}