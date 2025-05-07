#include "wall.h"

wall::wall(int row, int col, objectType oType) : unmovingObject(row, col, oType) {
    hitsLeft = 2;
    // Constructor implementation
    // The member initializer list initializes the base class unmovingObject with x and y values.
    // It also initializes hitsLeft to 2.
}
wall::~wall(){
    
}