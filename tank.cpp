#include "tank.h"

tank::tank(int x, int y, float orientation, size_t tank_size) : movingObject(x, y, orientation), tank_size(tank_size), shotsLeft(16), inBackwords(0) {
    // Constructor implementation
    // The member initializer list initializes the base class movingObject with x, y, and orientation.
    // It also initializes the tank_size, shotsLeft, and inBackwords members.
}
tank::~tank() {
    // Destructor implementation
    // No specific cleanup is needed for this class, but the destructor is defined for completeness.
}