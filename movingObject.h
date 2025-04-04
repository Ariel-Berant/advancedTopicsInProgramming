#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H
#include "matrixObject.h"

class movingObject : matrixObject
{
protected:
    int orientation; // The orientation of the moving object in degrees.
    // The movingObject class inherits from matrixObject, allowing it to use the properties and methods of matrixObject.
    // This class represents an object that can move in a 2D space.
public:
    movingObject(int x, int y, int orientation);
};

#endif // MOVINGOBJECT_H