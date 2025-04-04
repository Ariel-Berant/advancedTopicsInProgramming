#include "unmovingObject.h"

unmovingObject::unmovingObject(int x, int y) : matrixObject(x, y) {
    // Constructor implementation
    // The member initializer list initializes the base class matrixObject with x and y values.
    // No additional code is needed here since the initialization is done in the initializer list.
}
unmovingObject::~unmovingObject() {
    // Destructor implementation
    // No specific cleanup is needed for this class, but the destructor is defined for completeness.
}