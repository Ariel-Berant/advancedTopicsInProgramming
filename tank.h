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
    // - inBackwards: An integer indicating whether the tank is moving backward or not.
    // - tank_size: A constant size_t representing the size of the tank.

    int shotsLeft;
    int inBackwards;
    int turnsUntilNextShot;


public:
    tank(int x, int y, orientation orient, objectType oType);
    void setOrientation(const orientation newOrient);
    bool canShoot();
    int* newLocationAtReverse(const int numOfCols, const int numOfRows) const;
    void updateTurnsUntilNextShot();
    void useShot();
    orientation getOrientation() const;
    int getInBack() const; // Returns the inBackwards status
    void setInBackwards(int inBack); // Sets the inBackwards status
};


#endif // TANK_H