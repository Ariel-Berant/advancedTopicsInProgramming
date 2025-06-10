#include "PlayerTankAlgorithm.h"

PlayerTankAlgorithm::PlayerTankAlgorithm(int row, int col, orientation orient, objectType oType) : movingObject(row, col,oType , orient), shotsLeft(-1), inBackwards(0), turnsUntilNextShot(0), calcMoveRound(0), currTurn(0){
    // Constructor implementation
    // The member initializer list initializes the base class movingObject with x, y, and orientation.
    // It also initializes the tank_size, shotsLeft, and inBackwards members.
}
PlayerTankAlgorithm::~PlayerTankAlgorithm(){
    // Destructor implementation
    // No specific cleanup is needed here since the base class destructor will handle it.
    // The destructor is declared virtual in the base class to ensure proper cleanup of derived classes.
}

int PlayerTankAlgorithm::getInBack() const {
    return inBackwards;
}

int PlayerTankAlgorithm::getNumOfShotsLeft() const {
    return shotsLeft;
}

void PlayerTankAlgorithm::setInBackwards(int inBack) {
    inBackwards = inBack;
}


bool PlayerTankAlgorithm::canShoot() const {
    return shotsLeft > 0 && turnsUntilNextShot == 0;
}

bool PlayerTankAlgorithm::hasBullets() const {
    return shotsLeft > 0;
}

void PlayerTankAlgorithm::useShot() {
    turnsUntilNextShot = 4;
    shotsLeft--;
}

void PlayerTankAlgorithm::updateTurn() {
    currTurn++;
    if(turnsUntilNextShot > 0){
        turnsUntilNextShot--;
    }
}
int PlayerTankAlgorithm::getCurrTurn() const {
    return currTurn;
}

bool PlayerTankAlgorithm::isSafe(const int col, const int row,
                  const int numOfCols, const int numOfRows, const int movesAhead, objectType type) const{
    matrixObject* unmovingObj = tankBattleInfo->getGameBoard()[col][row][0].get();
    matrixObject* bulletObj;

    // Check for walls or mines
    if (unmovingObj && (unmovingObj->getType() == W || unmovingObj->getType() == M)) {
        return false;
    }

    bulletObj = tankBattleInfo->getGameBoard()[col][row][1].get();
    if (bulletObj) {
        if ((bulletObj->getType() == P1T && type == P2T) || (bulletObj->getType() == P2T && type == P1T)) {
            return false;
        }        
    }
    
    // Check for bullets
    bool bulletNotFound = true;

    vector<array<int, 3>> possibleLocs = {
        {col,                                                  					(numOfRows + ((row - 2 * movesAhead) % numOfRows)) % numOfRows,          	U},
        {col,                                                  					(numOfRows + row + 2 * movesAhead) % numOfRows,          	                D},
        {(numOfCols + ((col - 2 * movesAhead) % numOfCols)) % numOfCols,    	row,                                                     	                L},
        {(numOfCols + col + 2 * movesAhead) % numOfCols,       					row,                                                     	                R},
        {(numOfCols + ((col - 2 * movesAhead) % numOfCols)) % numOfCols,    	(numOfRows + ((row - 2 * movesAhead) % numOfRows)) % numOfRows,          	UL},
        {(numOfCols + col + 2 * movesAhead) % numOfCols,       					(numOfRows + ((row - 2 * movesAhead) % numOfRows)) % numOfRows,          	UR},
        {(numOfCols + ((col - 2 * movesAhead) % numOfCols)) % numOfCols,    	(numOfRows + row + 2 * movesAhead) % numOfRows,          	                DL},
        {(numOfCols + col + 2 * movesAhead) % numOfCols,       					(numOfRows + row + 2 * movesAhead) % numOfRows,         	                DR},
        {col,                                                  					(numOfRows + ((row - 2 * movesAhead - 1) % numOfRows)) % numOfRows,      	U},
        {(numOfCols + ((col - 2 * movesAhead - 1) % numOfCols)) % numOfCols,  	row,                                                  		   			    L},
        {(numOfCols + ((col - 2 * movesAhead - 1) % numOfCols)) % numOfCols,  	(numOfRows + ((row - 2 * movesAhead - 1) % numOfRows)) % numOfRows,      	UL},
        {(numOfCols + ((col - 2 * movesAhead - 1) % numOfCols)) % numOfCols,  	(numOfRows + ((row + 2 * movesAhead - 1) % numOfRows)) % numOfRows,    	    DL},
        {(numOfCols + ((col + 2 * movesAhead - 1) % numOfCols)) % numOfCols,  	(numOfRows + (row - 2 * movesAhead - 1) % numOfRows) % numOfRows,          	            UR},
        {(numOfCols + col + 2 * movesAhead - 1) % numOfCols,       			    (numOfRows + ((row + 2 * movesAhead - 1) % numOfRows)) % numOfRows,      	DR},
        {(numOfCols + col + 2 * movesAhead - 1) % numOfCols,       			    row,                                                     	                R},
        {col,                                                      			    (numOfRows + row + 2 * movesAhead - 1) % numOfRows,          	            D}
    };
    for (array<int, 3> loc: possibleLocs) {
        bulletObj = tankBattleInfo->getGameBoard()[loc[0]][loc[1]][1].get();
        if (tankBattleInfo->getGameBoard()[loc[0]][loc[1]][1] != nullptr && (tankBattleInfo->getGameBoard()[loc[0]][loc[1]][1]->getType() == B)) {
            bulletNotFound = false;
        }
    }
    return bulletNotFound;
}


// Function to get the rotations needed to reach the desired orientation from the current orientation
vector<ActionRequest> PlayerTankAlgorithm::getRotations(orientation curr, orientation desired) const{
    vector<ActionRequest> rotations = vector<ActionRequest>(0);
    int diff = (8 + curr - desired) % 8;
    switch (diff) {
        case 2:
            rotations.push_back(ActionRequest::RotateLeft90);
            break;
        case 3:
            rotations.push_back(ActionRequest::RotateLeft90);
            rotations.push_back(ActionRequest::RotateLeft45);
            break;
        case 1:
            rotations.push_back(ActionRequest::RotateLeft45);
            break;
        case 6:
            rotations.push_back(ActionRequest::RotateRight90);
            break;
        case 5:
            rotations.push_back(ActionRequest::RotateRight90);
            rotations.push_back(ActionRequest::RotateRight45);
            break;
        case 7:
            rotations.push_back(ActionRequest::RotateRight45);
            break;
        case 4:
            rotations.push_back(ActionRequest::RotateLeft90);
            rotations.push_back(ActionRequest::RotateLeft90);
            break;
        default:
            break;
    }
    rotations.push_back(ActionRequest::MoveForward);
    return rotations;
}

bool PlayerTankAlgorithm::canSeeOtherTank(const int otherLoc[2], int numOfCols, int numOfRows) const {
    int move[2] = {0, 0}, currLoc[2] = {location[0], location[1]};
    // matrixObject* matObj;
    bool canSee = false;
    switch (orient){
        case UR:
            move[0] = 1;
            move[1] = -1;
            break;
        case U:
            move[1] = -1;
            break;
        case DL:
            move[0] = -1;
            move[1] = 1;
            break;
        case D:
            move[1] = 1;
            break;
        case UL:
            move[0] = -1;
            move[1] = -1;
            break;
        case L:
            move[0] = -1;
            break;
        case DR:
            move[0] = 1;
            move[1] = 1;
            break;
        case R:
            move[0] = 1;
            break;
        default:
            break;
    }
    do {
        currLoc[0] = (currLoc[0] + move[0] + numOfCols) % numOfCols;
        currLoc[1] = (currLoc[1] + move[1] + numOfRows) % numOfRows;
        if (currLoc[0] == otherLoc[0] && currLoc[1] == otherLoc[1]) {
            canSee = true; // Other tank found
            break;
        }
    } while (currLoc[0] != location[0] || currLoc[1] != location[1]);
    return canSee;
}


// Function to check if the tank is surrounded by walls, bullets or mines(if by tank, it will try to shoot it, not surrounded)
bool PlayerTankAlgorithm::isSurrounded(const int *tankLoc) const {
    // Check if the tank is surrounded by walls or bullets
    int numOfCols = tankBattleInfo->getGameBoard().size();
    int numOfRows = tankBattleInfo->getGameBoard()[0].size();
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue; // Skip the tank's own position
            int newCol = (tankLoc[0] + i + numOfCols) % numOfCols;
            int newRow = (tankLoc[1] + j + numOfRows) % numOfRows;
            if (tankBattleInfo->getGameBoard()[newCol][newRow][0] && tankBattleInfo->getGameBoard()[newCol][newRow][0]->getType() != W) {
                // No wall found
                if (tankBattleInfo->getGameBoard()[newCol][newRow][1] && tankBattleInfo->getGameBoard()[newCol][newRow][1]->getType() != B) {
                    // No bullet found
                    if (tankBattleInfo->getGameBoard()[newCol][newRow][1] && tankBattleInfo->getGameBoard()[newCol][newRow][1]->getType() != M) {
                        // No mine found - free space
                        return false; 
                    }
                }
            }
        }
    }
    return true; // Surrounded by walls, bullets or mines
}


// Function to calculate the target orientation based on position differences
int PlayerTankAlgorithm::calculateTargetOrientation(int targetCol, int targetRow) {
    // Calculate the offset
    int offsetRow = targetRow - location[1];
    int offsetCol = targetCol - location[0];

    // Handle the edge case where start equals target
    if (offsetRow > 0 && offsetCol > 0) {
        return DR;
    }
    else if (offsetRow < 0 && offsetCol < 0) {
        return UL;
    }
    else if (offsetRow == 0) {
        return (offsetCol > 0) ? R : L; // Horizontal movement
    }
    else if (offsetCol == 0) {
        return (offsetRow > 0) ? D : U; // Vertical movement
    }
    else if (offsetRow > 0 && offsetCol < 0) {
        return DL; // Down-Left
    }
    else if (offsetRow < 0 && offsetCol > 0) {
        return UR; // Up-Right
    }
    return 0; // Default case (should not happen)
}

// Function to determine the correct move to face the target
pair<ActionRequest, int> PlayerTankAlgorithm::determineNextMove(int currentOrientation, int targetOrientation) {
    if (targetOrientation == -1) {
          return {ActionRequest::DoNothing,0}; // No valid move if already at the target
      }
  
      // Calculate the difference in orientation
      int diff = (targetOrientation - currentOrientation + 8) % 8;
  
      // Determine the next move based on the difference in orientation
      if (diff == 0) {
          return {ActionRequest::MoveForward, 0}; // Already facing the target
      } else if (diff == 1) {
          return {ActionRequest::RotateRight45,1}; // 1 step clockwise
      } else if (diff >= 2 && diff <= 4) {
          return {ActionRequest::RotateRight90, 1}; // Beteen 2 to 4 steps clockwise
      } else if (diff == 5 || diff == 6) {
          return {ActionRequest::RotateLeft90, 2}; // 3 or 2 steps counterclockwise (5/8ths clockwise equivalent)
      }else if (diff == 7) {
          return {ActionRequest::RotateLeft45, 1}; // 1 step counterclockwise
      }
  
      return {ActionRequest::DoNothing,0}; // Default action if something goes wrong
  }


  pair<ActionRequest, int> PlayerTankAlgorithm::findAdjSafe(int numOfCols, int numOfRows, objectType type, int closestBulletDist){
    //search for a safest place among all the neighbors cells and return the fist move needed to get there 
    // int targetOrientation;
    for(int dir = 0 ;dir < 8 ; dir++){
        pair<int,int> pointToCheck = getNeighborPointGivenOrient(orientation(dir), numOfCols, numOfRows);
        // targetOrientation = calculateTargetOrientation(location[0], location[1], pointToCheck.first, pointToCheck.second);
        pair<ActionRequest, int> movesPair =  determineNextMove(orient, dir);
        int numOfMoves = movesPair.second;
        if(isSafe(pointToCheck.first, pointToCheck.second, numOfCols, numOfRows, numOfMoves, type)
                && isSafe(pointToCheck.first, pointToCheck.second, numOfCols, numOfRows, numOfMoves + 1, type)){
            if(numOfMoves + 1 <= (closestBulletDist/2)){
                return movesPair;
            }
        }
    }
    return {ActionRequest::DoNothing,0};
}


// Function to get the row and col offsets based on the direction
pair<int, int> PlayerTankAlgorithm::getDirectionOffset(orientation dir){
    switch (dir) {
        case 0: return {0, -1};  // U
        case 1: return {1, -1};  // UR
        case 2: return {1, 0};   // R
        case 3: return {1, 1};   // DR
        case 4: return {0, 1};   // D
        case 5: return {-1, 1};  // DL
        case 6: return {-1, 0};  // L
        case 7: return {-1, -1}; // UL
        default: return {0, 0};  // Default case (invalid direction)
    }
}

pair<int, int> PlayerTankAlgorithm::getNeighborPointGivenOrient(orientation dir, int numOfCols, int numOfROws) {
    pair<int, int> off = getDirectionOffset(dir);
    off.first = (off.first + location[0] + numOfCols) % numOfCols;
    off.second = (off.second + location[1] + numOfROws) % numOfROws;
    return off;
}

void PlayerTankAlgorithm::setNumOfShotsLeft(int numOfShots) {
    shotsLeft = numOfShots;
}

// void PlayerTankAlgorithm::updateBattleInfo(BattleInfo& info){
//     PlayerBattleInfo & battleInfoRef = dynamic_cast<PlayerBattleInfo&>(info);
//     for(auto currBullet = bulletsTankShot.begin(); currBullet != bulletsTankShot.end();){
//         pair<int,int> bulletOffset = getDirectionOffset((*currBullet)->getOrientation());
//         // because bullets move twice as tanks and because we already calculated a new location
//         //which the real bullet didnt reached yet, we need to find his real location
//         int bulletRealLocation[2] = {(*currBullet)->getLocation()[0] + bulletOffset.first, (*currBullet)->getLocation()[1] + bulletOffset.second};
//         auto& boardCell = battleInfoRef.getGameBoard()[bulletRealLocation[0]][bulletRealLocation[1]][1];
//         if(!boardCell || boardCell->getType() != B){
//             // if the location the bullet supposed to be is empty or has tank in it than the bullet had been destroyed
//             currBullet = bulletsTankShot.erase(currBullet);
//         }
//         else {
//             ++currBullet;
//         }
//     }
// }




bool PlayerTankAlgorithm::canMakeMove(ActionRequest moveChosen, int numOfCols, int numOfRows){
    unique_ptr<int[]> newLoc;
    if (moveChosen == ActionRequest::MoveForward){
        newLoc = newLocation(numOfCols, numOfRows);
        if (tankBattleInfo->getGameBoard()[newLoc[0]][newLoc[1]][0] && tankBattleInfo->getGameBoard()[newLoc[0]][newLoc[1]][0]->getType() == W){
            return false;
        }
        return true;
    }
    else if (moveChosen == ActionRequest::MoveBackward){
        newLoc = newLocation(numOfCols, numOfRows , true);
        if (tankBattleInfo->getGameBoard()[newLoc[0]][newLoc[1]][0] && tankBattleInfo->getGameBoard()[newLoc[0]][newLoc[1]][0]->getType() == W){
            return false;
        }
    }
    else if (moveChosen == ActionRequest::Shoot){
        if (!canShoot()){
            return false;
        }
    }
    return true;
}


void PlayerTankAlgorithm::waitingforBackwordMove(ActionRequest tanksMove, int numOfCols, int numOfRows){
    unique_ptr<int[]> tankNewLocation = nullptr;
    if(tanksMove == ActionRequest::MoveForward) {
        setInBackwards(0);
    }
    else {
        if (tanksMove != ActionRequest::DoNothing){
        }
        else { // If the tank is in backwards mode and tries to do nothing, we just increase the inBackwards counter
            setInBackwards(getInBack() + 1);
            if(getInBack() == 3){
                tankNewLocation = newLocation(numOfCols, numOfRows, true);
                tankBattleInfo->getGameBoard()[tankNewLocation[0]][tankNewLocation[1]][1] = std::move(tankBattleInfo->getGameBoard()[location[0]][location[1]][1]);
                //tankBattleInfo->getGameBoard()[location[0]][location[1]][1] = nullptr;
                setNewLocation(tankNewLocation[0], tankNewLocation[1]);
            }
        }
    }
}

void PlayerTankAlgorithm::moveForwardMove(bool tankCanMove ,/* ActionRequest tanksMove, */ int numOfCols, int numOfRows){
    unique_ptr<int[]> tankNewLocation = nullptr;
    setInBackwards(0);
    if (tankCanMove){
        tankNewLocation = newLocation(numOfCols, numOfRows);
        tankBattleInfo->getGameBoard()[tankNewLocation[0]][tankNewLocation[1]][1] = tankBattleInfo->getGameBoard()[location[0]][location[1]][1];
        tankBattleInfo->getGameBoard()[location[0]][location[1]][1] = nullptr;
        setNewLocation(tankNewLocation[0], tankNewLocation[1]);
    }
}

void PlayerTankAlgorithm::moveBackwardMove(bool tankCanMove ,/* ActionRequest tanksMove, */ int numOfCols, int numOfRows){
    unique_ptr<int[]> tankNewLocation = nullptr;
    if (tankCanMove){
        if (getInBack() >= 3){ // if we have moved backwards last turn and want to move
            setInBackwards(getInBack() + 1);
            tankNewLocation = newLocation(numOfCols, numOfRows, true);
            tankBattleInfo->getGameBoard()[tankNewLocation[0]][tankNewLocation[1]][1] = std::move(tankBattleInfo->getGameBoard()[location[0]][location[1]][1]);
            //tankBattleInfo->getGameBoard()[location[0]][location[1]][1] = nullptr;
            setNewLocation(tankNewLocation[0], tankNewLocation[1]);
        }
        else{
            setInBackwards(1);
        }
    }
    else{
        setInBackwards(0);
    }
}

void PlayerTankAlgorithm::shootMove(bool tankCanMove){
    if (tankCanMove){
        useShot();
        unique_ptr<int[]> bulletLocation = newLocation(tankBattleInfo->getGameBoard().size(), tankBattleInfo->getGameBoard()[0].size());
        if(checkIfBulletHitObject(bulletLocation[0], bulletLocation[1])){
            // If the bullet hit an object, do not add it to the bulletsTankShot vector
        }
        else{
            // Create a new bullet and add it to the bulletsTankShot vector
            bulletsTankShot.push_back(make_shared<bullet>(bullet(bulletLocation[0], bulletLocation[1], getOrientation(), B)));
            // Place the bullet on the game board
            tankBattleInfo->getGameBoard()[bulletLocation[0]][bulletLocation[1]][1] = make_shared<bullet>(bullet(bulletLocation[0], bulletLocation[1], getOrientation(), B));
        }
    }
}

orientation PlayerTankAlgorithm::calculateNewOrientation(ActionRequest &tanksMove){
    if(tanksMove == ActionRequest::RotateLeft45){
        return orientation((getOrientation()+ 7) % 8);
    }
    else if(tanksMove == ActionRequest::RotateRight45){
        return orientation((getOrientation() + 1) % 8);
    }
    else if(tanksMove == ActionRequest::RotateLeft90){
        return orientation((getOrientation() + 6) % 8);
    }
    else{//(tanksMove == ActionRequest::RotateRight90)
        return orientation((getOrientation() + 2) % 8);
    }
}

void PlayerTankAlgorithm::updateTankData(ActionRequest &tanksMove, int numOfCols, int numOfRows){
    if (getInBack() > 0 && getInBack() < 3){
        waitingforBackwordMove(tanksMove, numOfCols, numOfRows);
    }
    else{
        bool tankCanMove = canMakeMove(tanksMove, numOfCols, numOfRows);
        [[maybe_unused]] orientation ornt = getOrientation();
        switch (tanksMove){
            case ActionRequest::RotateLeft45:
            case ActionRequest::RotateRight45:
            case ActionRequest::RotateLeft90:
            case ActionRequest::RotateRight90:
                setOrientation(calculateNewOrientation(tanksMove));
                break;
            case ActionRequest::DoNothing:
                setInBackwards(0);
                break;
            case ActionRequest::MoveForward:
                moveForwardMove(tankCanMove, /* tanksMove, */ numOfCols, numOfRows);
                break;
            case ActionRequest::MoveBackward:
                moveBackwardMove(tankCanMove, /* tanksMove, */ numOfCols, numOfRows);
                break;
            case ActionRequest::Shoot:
                shootMove(tankCanMove);
                break;
            default:
                break;
        }
    }
}

bool PlayerTankAlgorithm::checkIfBulletHitObject(int col, int row) const {
    // Check if the bullet hit a wall
    if (tankBattleInfo->getGameBoard()[col][row][0] && tankBattleInfo->getGameBoard()[col][row][0]->getType() == W) {
        tankBattleInfo->getGameBoard()[col][row][0] = nullptr; // Remove the wall from the game board
        return true; // Bullet hit a wall
    }
    // Check if the bullet hit a moving object (tank)
    else if (tankBattleInfo->getGameBoard()[col][row][1]) {
        tankBattleInfo->getGameBoard()[col][row][1] = nullptr; // Remove the tank from the game board
        return true; // Bullet hit a tank
    }
    return false; // No hit
}

void PlayerTankAlgorithm::moveTankBullets(int numOfCols, int numOfRows) {
    // Iterate through the bullets and move them
    for (auto currBullet = bulletsTankShot.begin(); currBullet != bulletsTankShot.end(); ) {
        auto newBulletLocation = (*currBullet)->newLocation(numOfCols, numOfRows);

        if (checkIfBulletHitObject(newBulletLocation[0], newBulletLocation[1])) {
            // Remove bullet from the old cell
            tankBattleInfo->getGameBoard()[(*currBullet)->getLocation()[0]][(*currBullet)->getLocation()[1]][1] = nullptr;
            // Remove bullet from vector; erase returns the next valid iterator
            currBullet = bulletsTankShot.erase(currBullet);
            continue;
        } else {
            // Move the bullet to the new location
            int oldCol = (*currBullet)->getLocation()[0];
            int oldRow = (*currBullet)->getLocation()[1];
            std::shared_ptr<bullet> currBulletPtr = *currBullet;

            // Place the bullet on the new cell, remove from old cell
            tankBattleInfo->getGameBoard()[newBulletLocation[0]][newBulletLocation[1]][1] = currBulletPtr;
            tankBattleInfo->getGameBoard()[oldCol][oldRow][1] = nullptr;
            (*currBullet)->setNewLocation(newBulletLocation[0], newBulletLocation[1]);
        }
    }
}
bool PlayerTankAlgorithm::friendlyFireRisk(int numOfCols, int numOfRows){
    // Check if the tank can shoot without hitting tanks from his own team (there are no friendly tanks between him and the target)
    int targetCol = tankBattleInfo->getClosestEnemyTankCol();
    int targetRow = tankBattleInfo->getClosestEnemyTankRow();
    if (targetCol == -1 || targetRow == -1) {
        return false; // No enemy tank found
    }
    int currCol = location[0];
    int currRow = location[1];
    pair<int, int> offsetToCheck = getDirectionOffset(getOrientation());
    while (currCol != targetCol || currRow != targetRow){
        currCol = (currCol + offsetToCheck.first + numOfCols) % numOfCols; // Move in the direction of the target
        currRow = (currRow + offsetToCheck.second + numOfRows) % numOfRows; // Move in the direction of the target
        if (tankBattleInfo->getGameBoard()[currCol][currRow][1] && tankBattleInfo->getGameBoard()[currCol][currRow][1]->getType() == oType){
            return true; // Friendly tank found in the way
        }
        if(tankBattleInfo->getGameBoard()[currCol][currRow][0] && tankBattleInfo->getGameBoard()[currCol][currRow][0]->getType() == W){
            break; // Wall found in the way, cant hurt a friendly tank
        }
    }
    return false; // Can shoot at the enemy tank
}



bool PlayerTankAlgorithm::checkIfOnSameLine(const int *otherLoc) const {
    if(abs(location[0] - otherLoc[0]) == abs(location[1] - otherLoc[1])){ 
        // check if both tanks are on the same diagonal - linear function, bias cancels out, and slope is 1
        return true;
    }
    if (location[0] == otherLoc[0] || location[1] == otherLoc[1]) { //check if both tanks are on the same row or column
        return true;
    }
    return false;
}