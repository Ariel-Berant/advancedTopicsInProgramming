#ifndef TANK_H
#define TANK_H

#include "movingObject.h"
#include "move.h"
#include "bullet.h"

using namespace std;

class tank : public movingObject
{
    // The tank class inherits from movingObject, allowing it to use the properties and methods of movingObject.
    // This class represents a tank object in a 2D space.
    
    // The tank class has the following properties:
    // - orientation: A float representing the orientation of the tank.
    // - shotsLeft: An integer representing the number of shots left for the tank.
    // - inBackwards: An integer indicating whether the tank is moving backward or not.
    // - tank_size: A constant size_t representing the size of the tank.
protected:
    int shotsLeft;
    int inBackwards;
    int turnsUntilNextShot;
    int calcMoveRound;
    vector<objMove> moves;
    bool isSurrounded(const vector<vector<array<matrixObject *, 3>>> &gameBoard, const int *tank2Loc) const;
    pair<objMove, int> determineNextMove(int currentOrientation, int targetOrientation);
    int calculateTargetOrientation(int targetRow, int targetCol);
    pair<objMove, int> findAdjSafe(const vector<vector<array<matrixObject *, 3>>> &gameBoard, int numOfCols, int numOfRows, int closestBulletDist = 100);
    pair<int, int> getNeighborPointGivenOrient(int orient, int numOfROws, int numOfCols);
    pair<int, int> getDirectionOffset(int dir);
    


public:
    tank(int row, int col, orientation orient, objectType oType);
    ~tank() override;
    void setOrientation( orientation newOrient);
    bool canShoot() const;
    // int* newLocationAtReverse( int numOfCols,  int numOfRows) const;
    virtual void updateTurn();
    void useShot();
    int getInBack() const; // Returns the inBackwards status
    void setInBackwards(int inBack); // Sets the inBackwards status
    virtual objMove play(const vector<vector<array<matrixObject*, 3>>>& gameBoard, const int otherLoc[2], int numOfCols, int numOfRows) = 0; // Pure virtual function
    bool isSafe(int x, int y, const vector<vector<array<matrixObject*, 3>>>& gameBoard, int numOfCols, int numOfRows, int movesAhead) const;
    vector<objMove> getRotations(orientation start, orientation desired) const;
    bool canSeeOtherTank(const int otherLoc[2], int numOfRows, int numOfCols) const;
    bool hasBullets() const;
    int getNumOfShotsLeft() const;

};


#endif // TANK_H