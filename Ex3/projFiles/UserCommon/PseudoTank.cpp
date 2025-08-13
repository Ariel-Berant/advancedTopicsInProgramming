#include "PseudoTank.h"

using namespace UserCommon_0000;

PseudoTank::PseudoTank(int row, int col, objectType PseudoTankType, orientation orientVal, int numOfShots, int tankIndex, std::unique_ptr<TankAlgorithm> tankAlg)
        : movingObject(row, col, PseudoTankType, orientVal), shotsLeft(numOfShots), turnsUntilNextShot(0), turnsInBackwards(0), tankIndex(tankIndex), isDead(false), tankAlg(std::move(tankAlg)) {};

int PseudoTank::getTankIndex() const {
    return tankIndex;
}

int PseudoTank::getPlayerTankNum() const {
    return (getType() == P1T) ? 1 : 2; // Assuming P1T is player 1's tank and P2T is player 2's tank
}

// Getter and Setter for inBackwards
int PseudoTank::getInBackwards() const { return turnsInBackwards; }
void PseudoTank::setInBackwards(int val) { turnsInBackwards = val; }
int PseudoTank::getInBack() const { return turnsInBackwards; }

// Getter and Setter for shotsLeft
int PseudoTank::getShotsLeft() const { return shotsLeft; }
void PseudoTank::useShot() { --shotsLeft; turnsUntilNextShot = 4; }

bool PseudoTank::getIsDead() {
    return isDead;
}
void PseudoTank::setIsDead() {
    isDead = true;
}

// Getter and Setter for turnsFromLastShot
void PseudoTank::decrementTurnsFromLastShot() { 
    if(turnsUntilNextShot > 0) 
        --turnsUntilNextShot; 
    else 
        turnsUntilNextShot = 0;
}

// Getter and Setter for orientation (using inherited orient member)
orientation PseudoTank::getOrientation() const { return orient; }
void PseudoTank::setOrientation(int orientVal) { orient = static_cast<orientation>(orientVal); }

// canShoot function
bool PseudoTank::canShoot() const { return shotsLeft > 0 && turnsUntilNextShot == 0; }