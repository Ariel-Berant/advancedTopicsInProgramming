#ifndef PLAYERTANKALGORITHM_H
#define PLAYERTANKALGORITHM_H

#include "../UserCommon/MovingObject.h"
#include "../UserCommon/Bullet.h"
#include "../common/TankAlgorithm.h"
#include "../common/ActionRequest.h"
#include "PlayerBattleInfo.h"

using namespace UserCommon_0000;

namespace Algorithm_0000 {
    class PlayerTankAlgorithm : public movingObject , public TankAlgorithm
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
        int calcMoveRound;// The number of moves the current operation should take
        int currTurn;
        vector<ActionRequest> moves;
        unique_ptr<PlayerBattleInfo> tankBattleInfo;
        vector<shared_ptr<bullet>> bulletsTankShot; // Vector to store bullets the tank shot that currently in the air


        bool isSurrounded(const int *tank2Loc) const;
        bool checkIfOnSameLine(const int *otherLoc) const ;
        pair<ActionRequest, int> determineNextMove(int currentOrientation, int targetOrientation);
        int calculateTargetOrientation(int targetCol, int targetRow);
        pair<ActionRequest, int> findAdjSafe(int numOfCols, int numOfRows, objectType type, int closestBulletDist = 100);
        pair<int, int> getNeighborPointGivenOrient(orientation dir, int numOfCols, int numOfRows);
        pair<int, int> getDirectionOffset(orientation dir);
        orientation calculateNewOrientation(ActionRequest &tanksMove);
        void waitingforBackwordMove(ActionRequest tanksMove, int numOfCols, int numOfRows);
        void moveForwardMove(bool tankCanMove ,/* ActionRequest tanksMove, */ int numOfCols, int numOfRows);
        void moveBackwardMove(bool tankCanMove ,/* ActionRequest tanksMove, */ int numOfCols, int numOfRows);
        void shootMove(bool tankCanMove);
        void updateTankData(ActionRequest &tanksMove, int numOfCols, int numOfRows);
        bool canMakeMove(ActionRequest moveChosen, int numOfCols, int numOfRows);
        void moveTankBullets(int numOfCols, int numOfRows);
        bool checkIfBulletHitObject(int col, int row) const;
        bool friendlyFireRisk(int numOfCols, int numOfRows, int targetCol, int targetRow); // Checks if the tank can shoot without hitting tanks from
                                                                    // his own team (there are no tanks fire between him and the target), return false if it can shoot

        


    public:
        PlayerTankAlgorithm(int row, int col, orientation orient, objectType oType);
        ~PlayerTankAlgorithm() override;
        bool canShoot() const;
        virtual void updateTurn();
        void useShot();
        int getInBack() const; // Returns the inBackwards status
        void setInBackwards(int inBack); // Sets the inBackwards status
        virtual ActionRequest getAction() = 0; // Pure virtual function
        bool isSafe(int col, int row, int numOfCols, int numOfRows, int movesAhead, objectType type) const;
        vector<ActionRequest> getRotations(orientation start, orientation desired) const;
        bool canSeeOtherTank(const int otherLoc[2], int numOfCols, int numOfRows) const;
        bool hasBullets() const;
        int getNumOfShotsLeft() const;
        void updateBattleInfo(BattleInfo& info);
        int getCurrTurn() const;
        void setNumOfShotsLeft(int numOfShots);

    };

}
#endif // PLAYERTANKALGORITHM_H