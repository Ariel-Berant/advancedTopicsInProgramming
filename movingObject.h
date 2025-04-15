#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H
#include "matrixObject.h"
#include "orientation.h"
#include <array>
using namespace std;


class movingObject : public matrixObject
{
protected:
    orientation orient; // The orientation of the moving object in degrees.
    // The movingObject class inherits from matrixObject, allowing it to use the properties and methods of matrixObject.
    // This class represents an object that can move in a 2D space.
public:
    movingObject(int x, int y, orientation orient);
    ~movingObject();

    int *newLocation(int numOfCols, int numOfRows) const;
    void setLocation(int newX, int newY);
};

#endif // MOVINGOBJECT_H