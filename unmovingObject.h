#ifndef UNMOVINGOBJECT_H
#define UNMOVINGOBJECT_H
#include "matrixObject.h"

class unmovingObject : matrixObject
{
    // The unmovingObject class inherits from matrixObject, allowing it to use the properties and methods of matrixObject.
    // This class represents an object that cannot move in a 2D space.
    
    // The unmovingObject class has the following properties:
    // - isAlive: A boolean indicating whether the object is alive or not.
    // - canMove: A boolean indicating whether the object can move or not.
protected:
    int orientation; // The orientation of the moving object in degrees.
    // The movingObject class inherits from matrixObject, allowing it to use the properties and methods of matrixObject.
    // This class represents an object that can move in a 2D space.
public:
    unmovingObject(int x, int y);
};

#endif // UNMOVINGOBJECT_H