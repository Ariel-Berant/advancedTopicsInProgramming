#include "MovingObject.h"
#include "../common/TankAlgorithm.h"

class PseudoTank : public movingObject
{
private:
    int inBackwards = 0; 
    int shotsLeft = 0;
    int turnsFromLastShot = 0;
    int turnsInBackwards = 0;

public:
    std::unique_ptr<TankAlgorithm> tankAlg;
    PseudoTank(int row, int col, objectType PseudoTankType, orientation orientVal)
        : movingObject(row, col, PseudoTankType, orientVal), inBackwards(false), shotsLeft(0), turnsFromLastShot(0), turnsInBackwards(0) {};
    ~PseudoTank() = default;

    int getTankNum() const {
        return (getType() == P1T) ? 1 : 2; // Assuming P1T is player 1's tank and P2T is player 2's tank
    }

    // Getter and Setter for inBackwards
    int getInBackwards() const { return inBackwards; }
    void setInBackwards(int val) { inBackwards = val; }
    int getInBack() const {return turnsInBackwards;}


    // Getter and Setter for shotsLeft
    int getShotsLeft() const { return shotsLeft; }
    void setShotsLeft(int val) { shotsLeft = val; }
    void updateShotsLeft(int delta) { shotsLeft += delta; }
    void incrementShotsLeft() { ++shotsLeft; }
    void useShot() { --shotsLeft; turnsFromLastShot = 0; }
 
    // Getter and Setter for turnsFromLastShot
    int getTurnsFromLastShot() const { return turnsFromLastShot; }
    void setTurnsFromLastShot(int val) { turnsFromLastShot = val; }
    void updateTurnsFromLastShot(int delta) { turnsFromLastShot += delta; }
    void incrementTurnsFromLastShot() { ++turnsFromLastShot; }

    // Getter and Setter for orientation (using inherited orient member)
    orientation getOrientation() const { return orient; }
    void setOrientation(int orientVal) { orient = static_cast<orientation>(orientVal); }

    // canShoot function
    bool canShoot() const { return shotsLeft > 0 && turnsFromLastShot > 4; }
};
