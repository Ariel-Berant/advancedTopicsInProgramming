#include "MovingObject.h"
#include "../common/TankAlgorithm.h"

class PseudoTank : public movingObject
{
private:
    int inBackwards = 0; 
    int shotsLeft;
    int turnsUntilNextShot = 0;
    int turnsInBackwards = 0;
    int tankIndex; // This will be the serial number of the tank in his team (1 or 2)

public:
    std::unique_ptr<TankAlgorithm> tankAlg;
    PseudoTank(int row, int col, objectType PseudoTankType, orientation orientVal, int numOfShots, int tankIndex)
        : movingObject(row, col, PseudoTankType, orientVal), inBackwards(false), shotsLeft(numOfShots), turnsUntilNextShot(0), turnsInBackwards(0), tankIndex(tankIndex) {};
    ~PseudoTank() = default;

    int getTankIndex() const {
        return tankIndex;
    }
    int getPlayerTankNum() const {
        return (getType() == P1T) ? 1 : 2; // Assuming P1T is player 1's tank and P2T is player 2's tank
    }

    // Getter and Setter for inBackwards
    int getInBackwards() const { return inBackwards; }
    void setInBackwards(int val) { inBackwards = val; }
    int getInBack() const {return turnsInBackwards;}


    // Getter and Setter for shotsLeft
    int getShotsLeft() const { return shotsLeft; }
    void useShot() { --shotsLeft; turnsUntilNextShot = 4; }
 
    // Getter and Setter for turnsFromLastShot
    void decrementTurnsFromLastShot() { 
        if(turnsUntilNextShot > 0) 
            --turnsUntilNextShot; 
        else 
            turnsUntilNextShot = 0;
    }

    // Getter and Setter for orientation (using inherited orient member)
    orientation getOrientation() const { return orient; }
    void setOrientation(int orientVal) { orient = static_cast<orientation>(orientVal); }

    // canShoot function
    bool canShoot() const { return shotsLeft > 0 && turnsUntilNextShot == 0; }
};
