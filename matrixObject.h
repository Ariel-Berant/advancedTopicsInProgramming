#ifndef MATRIXOBJECT_H
#define MATRIXOBJECT_H
#define _USE_MATH_DEFINES
#include <cmath>
#include "objectType.h"
#include <vector>
#include <array>
#include <queue>
#include <unordered_set>
#include <string>
class matrixObject
{
protected:
    int location[2];
    // The location array holds the x and y coordinates of the object.
    int hitsLeft;
    //The hitsLeft variable indicates how many hits the object can get before it's destroyed
    objectType oType;
public:
    matrixObject();
    matrixObject(int row, int col, objectType objectType);
    virtual ~matrixObject(); // Destructor
    const int *getLocation() const;       // Returns a pointer to the location array
    void setLocation(int row, int col); // Sets the location of the object
    bool getIsAlive() const;        // Returns whether the object has been destroyed or not
    void takeAHit();
    objectType getType() const;
};
#endif // MATRIXOBJECT_H