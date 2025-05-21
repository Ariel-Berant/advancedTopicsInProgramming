#ifndef PLAYERTANKALGORITHM_H
#define PLAYERTANKALGORITHM_H

#include "movingObject.h"
#include "move.h"
#include "bullet.h"
#include "../common/TankAlgorithm.h"
#include "../common/ActionRequest.h"
#include "PlayerBattleInfo.h"

using namespace std;

class PlayerTankAlgorithm : public movingObject , TankAlgorithm
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
    int currTurn;
    vector<objMove> moves;
    unique_ptr<PlayerBattleInfo> tankBattleInfo;
    bool isSurrounded(const int *tank2Loc) const;
    pair<objMove, int> determineNextMove(int currentOrientation, int targetOrientation);
    int calculateTargetOrientation(int targetCol, int targetRow);
    pair<objMove, int> findAdjSafe(int numOfCols, int numOfRows, int closestBulletDist = 100);
    pair<int, int> getNeighborPointGivenOrient(int orient, int numOfCols, int numOfRows);
    pair<int, int> getDirectionOffset(int dir);
    


public:
    PlayerTankAlgorithm(int row, int col, orientation orient, objectType oType);
    ~PlayerTankAlgorithm() override;
    void setOrientation( orientation newOrient);
    bool canShoot() const;
    virtual void updateTurn();
    void useShot();
    int getInBack() const; // Returns the inBackwards status
    void setInBackwards(int inBack); // Sets the inBackwards status
    virtual objMove play(const int otherLoc[2], int numOfCols, int numOfRows) = 0; // Pure virtual function
    bool isSafe(int col, int row, int numOfCols, int numOfRows, int movesAhead) const;
    vector<objMove> getRotations(orientation start, orientation desired) const;
    bool canSeeOtherTank(const int otherLoc[2], int numOfCols, int numOfRows) const;
    bool hasBullets() const;
    int getNumOfShotsLeft() const;
    void updateBattleInfo(BattleInfo& info) override;
    int getCurrTurn() const;
    void setNumOfShotsLeft(int numOfShots);

};


#endif // PLAYERTANKALGORITHM_H