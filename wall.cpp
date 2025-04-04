#include "wall.h"

wall::wall(int x, int y) : unmovingObject(x, y), hitsLeft(2) {
    // Constructor implementation
    // The member initializer list initializes the base class unmovingObject with x and y values.
    // It also initializes hitsLeft to 2.
}
wall::~wall() {
    // Destructor implementation
    // No specific cleanup is needed for this class, but the destructor is defined for completeness.
}