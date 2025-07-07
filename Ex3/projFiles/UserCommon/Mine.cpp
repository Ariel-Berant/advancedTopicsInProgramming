#include "Mine.h"

mine::mine(int row, int col, objectType oType) : unmovingObject(row, col, oType) {
    // Constructor implementation
    // The member initializer list initializes the base class unmovingObject with x and y values.
    // It also initializes hitsLeft to 2.
}
mine::~mine(){
    
}