#ifndef TANK_H
#define TANK_H

#include "movingObject.h"

class tank : public movingObject
{
    // The tank class inherits from movingObject, allowing it to use the properties and methods of movingObject.
    // This class represents a tank object in a 2D space.
    
    // The tank class has the following properties:
    // - orientation: A float representing the orientation of the tank.
    // - shotsLeft: An integer representing the number of shots left for the tank.
    // - inBackwords: An integer indicating whether the tank is moving backward or not.
    // - tank_size: A constant size_t representing the size of the tank.

    int shotsLeft;
    int inBackwords;

public:
    tank(int x, int y, orientation orient);
    void setOrientation(orientation newOrient);
};


#endif // TANK_H