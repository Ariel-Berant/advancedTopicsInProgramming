#ifndef MATRIXOBJECT_H
#define MATRIXOBJECT_H
#include "objectType.h"
class matrixObject
{
protected:
    int location[2];
    // The location array holds the x and y coordinates of the object.
    bool isAlive = true; // Initialize isAlive to true
    // The isAlive variable indicates whether the object is alive or not.
    bool canMove = false; // Initialize canMove to false
    // The canMove variable indicates whether the object can move or not.
    int hitsLeft;
    //The hitsLeft variable indicates how many hits the object can get before it's destroyed
    objectType oType;
public:
    matrixObject();
    matrixObject(int x, int y, objectType objectType);
    ~matrixObject();
    const int *getLocation() const;       // Returns a pointer to the location array
    void setLocation(int x, int y); // Sets the location of the object
    bool getIsAlive() const;        // Returns whether the object has been destroyed or not
    bool getCanMove() const;        // Returns the canMove status
    void takeAHit();
    objectType getType() const;
};
#endif // MATRIXOBJECT_H