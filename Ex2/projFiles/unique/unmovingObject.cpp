#include "unmovingObject.h"

unmovingObject::unmovingObject(int row, int col, objectType oType) : matrixObject(row, col, oType) {
    // Constructor implementation
    // The member initializer list initializes the base class matrixObject with x and y values.
    // No additional code is needed here since the initialization is done in the initializer list.
}

unmovingObject::~unmovingObject() {
    // Destructor implementation
    // No specific cleanup is needed for unmovingObject, but this can be overridden in derived classes if necessary.
}