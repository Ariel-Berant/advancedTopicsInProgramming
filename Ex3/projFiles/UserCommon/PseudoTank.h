#pragma once

#include "MovingObject.h"
#include "../common/TankAlgorithm.h"

using namespace UserCommon_0000;

class PseudoTank : public movingObject
{
private:
    int shotsLeft;
    int turnsUntilNextShot = 0;
    int turnsInBackwards = 0;
    int tankIndex; // This will be the serial number of the tank in his team (1 or 2)
    int isDead;

public:
    std::unique_ptr<TankAlgorithm> tankAlg;
    PseudoTank(int row, int col, objectType PseudoTankType, orientation orientVal, int numOfShots, int tankIndex);
    ~PseudoTank() = default;

    int getTankIndex() const;
    int getPlayerTankNum() const;
    int getInBackwards() const;
    void setInBackwards(int val);
    int getInBack() const;
    int getShotsLeft() const;
    void useShot();
    bool getIsDead();
    void setIsDead();
    void decrementTurnsFromLastShot();
    orientation getOrientation() const;
    void setOrientation(int orientVal);
    bool canShoot() const;
};
