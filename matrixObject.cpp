#include "matrixObject.h"

matrixObject::matrixObject(int x, int y) : location{x, y} {
    // Constructor implementation
    // The member initializer list initializes the location array with the provided x and y values.
    // No additional code is needed here since the initialization is done in the initializer list.
}

matrixObject::~matrixObject() {
    // Destructor implementation
    // No specific cleanup is needed for this class, but the destructor is defined for completeness.
}

int* const matrixObject::getLocation() {
    // Returns a pointer to the location array
    return location; // Use const_cast to remove constness for returning the pointer
}