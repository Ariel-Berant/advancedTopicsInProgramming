#include "mine.h"

mine::mine(int x, int y, objectType oType) : unmovingObject(x, y, oType) {
    // Constructor implementation
    // The member initializer list initializes the base class unmovingObject with x and y values.
    // It also initializes hitsLeft to 2.
}
mine::~mine() = default;