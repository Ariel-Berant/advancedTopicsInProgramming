#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H
#include "MatrixObject.h"
#include "Orientation.h"
using namespace std;


class movingObject : public matrixObject
{
protected:
    orientation orient; // The orientation of the moving object in degrees.
    int oldLocation[2];

    // The movingObject class inherits from matrixObject, allowing it to use the properties and methods of matrixObject.
    // This class represents an object that can move in a 2D space.
public:
    movingObject(int row, int col,objectType oType, orientation orient);
    virtual ~movingObject();

    void setOrientation( orientation newOrient);

    unique_ptr<int[]> newLocation(int numOfCols, int numOfRows, bool atReverse = false) const;
    const int *getOldLocation() const;
    void setNewLocation(int newRow, int newCol);
    orientation getOrientation() const;
};

#endif // MOVINGOBJECT_H