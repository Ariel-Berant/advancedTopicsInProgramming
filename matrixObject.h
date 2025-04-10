#ifndef MATRIXOBJECT_H
#define MATRIXOBJECT_H
class matrixObject
{
protected:
    int location[2];
    // The location array holds the x and y coordinates of the object.
    bool isAlive = true; // Initialize isAlive to true
    // The isAlive variable indicates whether the object is alive or not.
    bool canMove = false; // Initialize canMove to false
    // The canMove variable indicates whether the object can move or not.
public:
    matrixObject(int x, int y);
    ~matrixObject();
    const int *getLocation() const;       // Returns a pointer to the location array
    void setLocation(int x, int y); // Sets the location of the object
    bool getIsAlive() const;        // Returns the isAlive status
    bool getCanMove() const;        // Returns the canMove status
};
#endif // MATRIXOBJECT_H