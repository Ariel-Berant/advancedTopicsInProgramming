#include "GameManager.h"

#define INP_ERR_FILE "input_errors.txt"
#define LOG_FILE "game_log.txt"
const int MAX_STEPS_WITHOUT_SHELLS = 40;

using namespace std;


using namespace GameManager_0000;

REGISTER_GAME_MANAGER(gameManager_0000);

bool gameManager_0000::isValidFile(const string &filename) {
    filesystem::path filePath = filename;
    if (filePath.extension() != ".txt") {
        writeToFile("Error: The file '" + filename + "' is not a .txt file.\n", INP_ERR_FILE);
        return false;
    }
    else if (!filesystem::exists(filePath)) {
        writeToFile("Error: The file '" + filename + "' does not exist.\n", INP_ERR_FILE);
        return false;
    }
    
    return true;
}

bool gameManager_0000::parseGameInfo(const string line, const string description, int rowNum) {
    string fileRow = line;
    if (fileRow.empty()) {
        writeToFile("Error: The file '" + description + "' is empty.\n", INP_ERR_FILE);
        return false;
    }
    // Remove all whitespace characters from the string
    fileRow.erase(remove_if(fileRow.begin(), 
                              fileRow.end(),
                              [](unsigned char x) { return std::isspace(x); }),
               fileRow.end());
    if (fileRow.find(description) != 0) {
        writeToFile("Error: The row for \"" + description + "\" does not start with the expected description.\n", INP_ERR_FILE);
        return false;
    }
    fileRow.erase(0, description.length());
    // Check if the remaining string is a valid integer
    if (!all_of(fileRow.begin(), fileRow.end(), ::isdigit)) {
        writeToFile("Error: The row for \"" + description + "\" contains invalid characters.\nCorrect use is: "
             + description + "<NUM>\n", INP_ERR_FILE);
        return false;
    }
    int value = stoi(fileRow);
    switch (rowNum){
    case 1:
        maxTurns = 2 * value;
        break;
    case 2:
        maxBullets = value;
        break;
    case 3:
        numOfRows = value;
        break;
    case 4:
        numOfCols = value;
        break;
    default:
        // Not a valid row number
        break;
    }
    return true;
}

bool gameManager_0000::getRowsAndColsFromFile(const string &filename){
    string line;
    string descriptions[] = {"MaxSteps=", "NumShells=", "Rows=", "Cols="};
    bool gotDims = true;

    ifstream file1;
    file1.open(filename);
    if (!file1.is_open())
    {
        writeToFile("Error: Could not open the file '" + filename + "'.\n", INP_ERR_FILE);
        return false;
    }
    
    getline(file1, line); // Skip the first line with the map name
    for (int i = 1; i < 5; ++i)
    {
        if (getline(file1, line))
        {
            if (!parseGameInfo(line, descriptions[i - 1], i)){
                gotDims = false;
                break;
            }
        }
        else
        {
            writeToFile("Error: Failed to read line from file.\n", INP_ERR_FILE);
            gotDims = false;
        }
    }
    file1.close();

    return gotDims;
}


bool gameManager_0000::addTankToMap(int playerNum, int currCol, int currRow, TankAlgorithmFactory &tankFactory){
    orientation ornt = playerNum == 1 ? L : R;
    objectType type = playerNum == 1 ? P1T : P2T;
    int newTankIndex = playerNum == 1 ? ++numOfP1TanksLeft : ++numOfP2TanksLeft;
    shared_ptr<PseudoTank> tank = make_shared<PseudoTank>(currRow, currCol, type, ornt, maxBullets, newTankIndex, std::move(tankFactory(playerNum, newTankIndex)));
    if(!tank->tankAlg){
        writeToFile("Error: Failed to create tank algorithm.\n", INP_ERR_FILE);
        return false;
    }
    tanks.push_back(tank);
    printToLogVector.emplace_back("");
    currMovingObjects.push_back(tank);

    (*gameBoard)[currCol][currRow][1] = tank;
    return true;
}

void gameManager_0000::addUnmovingObjectToMap(char UnmovingObjectType, int currCol, int currRow){
    if(UnmovingObjectType == '#'){
        (*gameBoard)[currCol][currRow][0] = make_unique<wall>(currRow, currCol, W);
        numOfWalls++;
    }
    else if(UnmovingObjectType == '@'){
        (*gameBoard)[currCol][currRow][0] = make_unique<mine>(currRow, currCol, M);
        numOfMines++;
    }
}



bool gameManager_0000::createMap(const SatelliteView &map, TankAlgorithmFactory tankFactory1, TankAlgorithmFactory tankFactory2){
    for (int currRow = 0; currRow < numOfRows; currRow++) {
        for (int currCol = 0; currCol < numOfCols; currCol++) {
            char ch = map.getObjectAt(currCol, currRow);
            switch (ch) {
            case '#':
                addUnmovingObjectToMap('#', currCol, currRow);
                break;
            case '1':
                if(!addTankToMap(1, currCol, currRow, tankFactory1)){
                    return false;
                }
                break;
            case '2':
                if(!addTankToMap(2, currCol, currRow, tankFactory2)){
                    return false;
                }
                break;
            case '@':
                addUnmovingObjectToMap('@', currCol, currRow);
                break;
            case ' ':
            case '&':
                break;
            default:
                writeToFile("Error: unrecognized character, ASCII #'" + to_string(ch) + "' in the map file.\nPutting ' ' instead.\n", INP_ERR_FILE);
            }
        }
    }

    return true;
}


// Function initialize the game data from a given pathname.
// If some critical error occurs(criticalErr==true), function fails and returns false(i.e return !criticalErr).
bool gameManager_0000::initializeGame(const SatelliteView &map, TankAlgorithmFactory tankFactory1, TankAlgorithmFactory tankFactory2){
    if (gameBoard){
        gameBoard = nullptr;
    }
    if (!tanks.empty()){
        tanks.clear();
    }

    numOfP1TanksLeft = 0, numOfP2TanksLeft = 0;
    gameBoard = std::make_unique<vector<vector<array<shared_ptr<matrixObject>, 3>>>>(
        numOfCols, vector<array<shared_ptr<matrixObject>, 3>>(numOfRows, {nullptr, nullptr, nullptr}));
    if(createMap(map, tankFactory1, tankFactory2) == false){
        return false;
    }
    if(numOfP1TanksLeft == 0 || numOfP2TanksLeft == 0){
        printGameResultToLog();
        gameOver = true;
    }
    numOfP1Tanks = numOfP1TanksLeft; // Store the initial number of tanks for player 1
    numOfP2Tanks = numOfP2TanksLeft; // Store the initial number of tanks for player 2
    numOfBulletsLeft = maxBullets * (numOfP1TanksLeft + numOfP2TanksLeft); // Calculate the starting number of bullets
    return true;
}

int setOrientation(/*int tankIndexToDefine, */ActionRequest action, orientation &orient) {
    if (action == ActionRequest::RotateLeft45) {
        orient = static_cast<orientation>((orient + 7) % 8);
    } else if (action == ActionRequest::RotateRight45) {
        orient = static_cast<orientation>((orient + 1) % 8);
    } else if (action == ActionRequest::RotateLeft90) {
        orient = static_cast<orientation>((orient + 6) % 8);
    } else if (action == ActionRequest::RotateRight90) {
        orient = static_cast<orientation>((orient + 2) % 8);
    }
    return orient;
}

// Function to convert orientation enum to string
std::string orientationToString(orientation o) {
    switch (o) {
        case U: return "U";
        case UR: return "UR";
        case R: return "R";
        case DR: return "DR";
        case D: return "D";
        case DL: return "DL";
        case L: return "L";
        case UL: return "UL";
        default: return "Unknown";
    }
}

static std::string ActionRequestToString(ActionRequest action) {
    switch (action) {
        case ActionRequest::RotateLeft45: return "RotateLeft45";
        case ActionRequest::RotateRight45: return "RotateRight45";
        case ActionRequest::RotateLeft90: return "RotateLeft90";
        case ActionRequest::RotateRight90: return "RotateRight90";
        case ActionRequest::MoveForward: return "MoveForward";
        case ActionRequest::MoveBackward: return "MoveBackward";
        case ActionRequest::Shoot: return "Shoot";
        case ActionRequest::DoNothing: return "DoNothing";
        default: return "GetBattleInfo";
    }
}

void gameManager_0000::moveBullets(){
    unique_ptr<int[]> newLoc;
    for (shared_ptr<bullet> b : bullets) {
        newLoc = b->newLocation(numOfCols, numOfRows);
        b->setNewLocation(newLoc[1], newLoc[0]);
    }
}

// Helper: Check if two points are the same
bool isSamePoint(const int *a, const int *b){
    return a[0] == b[0] && a[1] == b[1];
}

// The next two functions were written with the help of ChatGPT

// Helper: Check if three points are collinear
bool areCollinear(const int *p, const int *q, const int *r){
    return (q[1] - p[1]) * (r[0] - q[0]) == (q[0] - p[0]) * (r[1] - q[1]);
}

bool passingBy(const int *aStart, const int *aEnd, const int *bStart, const int *bEnd){
    // Check if the two collinear segments passing by each other
    return isSamePoint(aStart, bEnd) && isSamePoint(aEnd, bStart);
    
}

// // Helper function to check if two line segments intersect
bool doLinesIntersect(const int *p1, const int *q1, const int *p2, const int *q2)
{
    auto orientation = [](const int *p, const int *q, const int *r)
    {
        int val = (q[1] - p[1]) * (r[0] - q[0]) - (q[0] - p[0]) * (r[1] - q[1]);
        if (val == 0)
            return 0;             // Collinear
        return (val > 0) ? 1 : 2; // Clockwise or Counterclockwise
    };

    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special cases (collinear points)
    auto onSegment = [](const int *p, const int *q, const int *r)
    {
        return q[0] <= max(p[0], r[0]) && q[0] >= min(p[0], r[0]) &&
               q[1] <= max(p[1], r[1]) && q[1] >= min(p[1], r[1]);
    };

    if (o1 == 0 && onSegment(p1, p2, q1))
        return true;
    if (o2 == 0 && onSegment(p1, q2, q1))
        return true;
    if (o3 == 0 && onSegment(p2, p1, q2))
        return true;
    if (o4 == 0 && onSegment(p2, q1, q2))
        return true;

    return false;
}

bool checkIfTank(const movingObject &object1){
    if(object1.getType() == P1T || object1.getType() == P2T){
        return true;
    }
    return false;
}

/* void gameManager_0000::printCollisionsToLog(const movingObject &object1, const movingObject &object2) const{
    if(checkIfTank(object1) && checkIfTank(object2) && (object1.getIsAlive() || object2.getIsAlive())){
        if at least one of the tanks has been destroyed by a collision between them, and we didn't notified about it
        int object1Player = object1.getType() == P1T ? 1 : 2;
        int object2Player = object2.getType() == P1T ? 1 : 2;
        int object1TankNum = dynamic_cast<const PseudoTank*>(&object1)->getTankIndex();
        int object2TankNum = dynamic_cast<const PseudoTank*>(&object2)->getTankIndex();
        writeToFile("Tank number " + to_string(object1TankNum) + " of player number " + to_string(object1Player) + " collided with tank number "
         + to_string(object2TankNum) + " of player number " + to_string(object2Player) + " at (" + to_string(object1.getOldLocation()[0]) + "," + to_string(object1.getOldLocation()[1]) + ").\n", LOG_FILE);
    }
    if (object1.getIsAlive() && object2.getIsAlive()){// we need to notify that the two objects been destroyed
        if (checkIfTank(object1) && object2.getType() == B){
            int tankPlayer = object1.getType() == P1T ? 1 : 2;
            int objectTankNum = dynamic_cast<const PseudoTank*>(&object1)->getTankIndex();
            writeToFile("Tank number " + to_string(objectTankNum) + " of player number " + to_string(tankPlayer) 
                + " got hit by a bullet at (" + to_string(object1.getOldLocation()[0]) + "," + to_string(object1.getOldLocation()[1]) + ").\n", LOG_FILE);
        }
        else if(object1.getType() == B && checkIfTank(object2)){
            int tankPlayer = object2.getType() == P1T ? 1 : 2;
            int objectTankNum = dynamic_cast<const PseudoTank*>(&object2)->getTankIndex();
            writeToFile("Tank number " + to_string(objectTankNum) + " of player number " + to_string(tankPlayer) 
                + " got hit by a bullet at " + to_string(object1.getOldLocation()[0]) + "," + to_string(object1.getOldLocation()[1]) + ".\n", LOG_FILE);
        }
        else if (object1.getType() == B && object2.getType() == B){
            writeToFile("The bullet at (" + (to_string(object1.getOldLocation()[0]) + "," + (to_string(object1.getOldLocation()[1])) + ") collided with the bullet at (" +
                         to_string(object2.getOldLocation()[0])) + "," + (to_string(object2.getOldLocation()[1])) + ") destroying each other.\n", LOG_FILE);
        }
    }
    else if (!object1.getIsAlive() && !object2.getIsAlive()){
        // we already notified that the two objects been destroyed thus we don't need to notify again
    }
    else if ((object1.getIsAlive() && object1.getType() == B) || (object2.getIsAlive() && object2.getType() == B)){// we need to notify that an object been destroyed
            writeToFile("A bullet has been destroyed by collision at (" + to_string(object1.getOldLocation()[0]) + "," + to_string(object1.getOldLocation()[1]) + ").\n", LOG_FILE);
    }
} */

// This function was written with the help of chat gpt
// Function to check for collisions between moving object1
bool gameManager_0000::checkCollisions(){
    for (size_t i = 0; i < currMovingObjects.size(); i++){
        for (size_t j = i + 1; j < currMovingObjects.size(); j++){
            const int *a_start = currMovingObjects[i]->getOldLocation();
            const int *a_end = currMovingObjects[i]->getLocation();
            const int *b_start = currMovingObjects[j]->getOldLocation();
            const int *b_end = currMovingObjects[j]->getLocation();
            // Check if they only touch at a start point
            bool touchesAtStartOnly =
            ((isSamePoint(a_start, b_end) && !isSamePoint(a_end, b_start)) ||
            (isSamePoint(a_end, b_start) && !isSamePoint(a_start, b_end)))&& !isSamePoint(a_end, b_end);

            if (doLinesIntersect(a_start, a_end, b_start, b_end) && !touchesAtStartOnly){
                // Check if the segments are collinear
                bool collinear =
                    areCollinear(a_start, a_end, b_start) &&
                    areCollinear(a_start, a_end, b_end) && passingBy(a_start, a_end, b_start, b_end);

                bool endAtSamePoint =
                    isSamePoint(a_end, b_end);

                // If they don't touch only at the start points and are collinear, they collide
                // and we need to check if they don't finish at the same point
                if (collinear || endAtSamePoint){
                    printCollisionsToLog(*currMovingObjects[i], *currMovingObjects[j]);
                    currMovingObjects[i]->takeAHit();
                    currMovingObjects[j]->takeAHit();
                }
            }
        }
    }

    return true;
}

 
void gameManager_0000::waitingforBackwordMove(ActionRequest tanksMove, int i){
    unique_ptr<int[]> tankNewLocation = nullptr;
    // int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2;
    // int tankNum = tanks[i]->getTankIndex();
    if(tanksMove == ActionRequest::MoveForward) {
        tanks[i]->setInBackwards(0);
        // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at ()" + (to_string(tanks[i]->getLocation()[0]) + "," +
        //                               to_string(tanks[i]->getLocation()[1])) + ") stayed in place.\n", LOG_FILE);

         printToLogVector[i] = "MoveForward";
    }
    else {
        if (tanksMove != ActionRequest::DoNothing){
            // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
            //                           to_string(tanks[i]->getLocation()[1])) + ") tried to make a move while in backwards mode.\n", LOG_FILE);
            printToLogVector[i] = ActionRequestToString(tanksMove) + " (ignored)";
        }
        else { // If the tank is in backwards mode and tries to do nothing, we just increase the inBackwards counter
            // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
            //                           to_string(tanks[i]->getLocation()[1])) + ") is still waiting for a backwards move.\n", LOG_FILE);
            printToLogVector[i] = ActionRequestToString(tanksMove);
        }
        
        tanks[i]->setInBackwards(tanks[i]->getInBack() + 1);
            if(tanks[i]->getInBack() == 3){
                tankNewLocation = tanks[i]->newLocation(numOfCols, numOfRows, true);
                tanks[i]->setNewLocation(tankNewLocation[1], tankNewLocation[0]);
                // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + to_string(tanks[i]->getOldLocation()[0]) + "," + to_string(tanks[i]->getOldLocation()[1])
                //              + ") moved backwards to (" + to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]) + ").\n", LOG_FILE);
            }
            else{
                // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                //                       to_string(tanks[i]->getLocation()[1])) + ") stayed in place waiting until he can move backwards.\n", LOG_FILE);
            }
    }
}

void gameManager_0000::moveForwardMove(bool tankCanMove ,/*ActionRequest tanksMove,*/ int i){
    // int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2;
    // int tankNum = tanks[i]->getTankIndex();
    unique_ptr<int[]> tankNewLocation = nullptr;
    tanks[i]->setInBackwards(0);
    if (tankCanMove){
            tankNewLocation = tanks[i]->newLocation(numOfCols, numOfRows);
            tanks[i]->setNewLocation(tankNewLocation[1], tankNewLocation[0]);
            // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + to_string(tanks[i]->getOldLocation()[0]) + "," + to_string(tanks[i]->getOldLocation()[1])
            //                 + ") went forwards to (" + to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]) + ").\n", LOG_FILE);
    }
    else{
        // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
        //                         to_string(tanks[i]->getLocation()[1])) + ") tried to move forwards when he couldn't (unstoppable force met an immovable object).\n", LOG_FILE);
    }
}

void gameManager_0000::moveBackwardMove(bool tankCanMove ,/* ActionRequest tanksMove,  */int i){
    // int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2;
    // int tankNum = tanks[i]->getTankIndex();
    unique_ptr<int[]> tankNewLocation = nullptr;
    if (tankCanMove){
        if (tanks[i]->getInBack() >= 3){ // if we have moved backwards last turn and want to move
            tanks[i]->setInBackwards(tanks[i]->getInBack() + 1);
            tankNewLocation = tanks[i]->newLocation(numOfCols, numOfRows, true);
            tanks[i]->setNewLocation(tankNewLocation[1], tankNewLocation[0]);
            // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + to_string(tanks[i]->getOldLocation()[0]) + "," + to_string(tanks[i]->getOldLocation()[1])
            //                 + ") moved backwards to (" + to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]) + ").\n", LOG_FILE);
        }
        else{
            tanks[i]->setInBackwards(1);
            //    writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
            //                           to_string(tanks[i]->getLocation()[1])) + ") stayed in place waiting until he can move backwards.\n", LOG_FILE);
        }
    }
    else{
        // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
        //                         to_string(tanks[i]->getLocation()[1])) + ") tried to move backwards when he couldn't (unstoppable force met an immovable object).\n", LOG_FILE);
        tanks[i]->setInBackwards(0);
    }
}

void gameManager_0000::shootMove(bool tankCanMove,/*  ActionRequest tanksMove,  */int i){
    // int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2;
    // int tankNum = tanks[i]->getTankIndex();
    unique_ptr<int[]> newBulletLocation = nullptr;
    if (tankCanMove){
        newBulletLocation = tanks[i]->newLocation(numOfCols, numOfRows);               // Get bullet location
        shared_ptr<bullet> b = make_shared<bullet>(newBulletLocation[1], newBulletLocation[0], tanks[i]->getOrientation(), B); // Create bullet
        currMovingObjects.emplace_back(b);
        bullets.emplace_back(b);
        tanks[i]->useShot();
        numOfBulletsLeft--;
        // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
        //                 to_string(tanks[i]->getLocation()[1])) + ") shot a bullet.\n", LOG_FILE);
        // writeToFile("The new bullet starting location is (" + (to_string(b->getLocation()[0]) + "," + to_string(b->getLocation()[1])) + ").\n", LOG_FILE);
    }
    else{
        // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
        //                 to_string(tanks[i]->getLocation()[1])) + ") shot a blank.\n", LOG_FILE);
    }
}


orientation gameManager_0000::calculateNewOrientation(ActionRequest &tanksMove, int i){
    if(tanksMove == ActionRequest::RotateLeft45){
        return orientation((tanks[i]->getOrientation()+ 7) % 8);
    }
    else if(tanksMove == ActionRequest::RotateRight45){
        return orientation((tanks[i]->getOrientation() + 1) % 8);
    }
    else if(tanksMove == ActionRequest::RotateLeft90){
        return orientation((tanks[i]->getOrientation() + 6) % 8);
    }
    else{//(tanksMove == ActionRequest::RotateRight90)
        return orientation((tanks[i]->getOrientation() + 2) % 8);
    }
}


void gameManager_0000::getTheIthTankMove(int i, ActionRequest &tanksMove){
    int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2;
    // int tankNum = tanks[i]->getTankIndex();
    // orientation ornt = tanks[i]->getOrientation();
    if (tanks[i]->getInBack() > 0 && tanks[i]->getInBack() < 3){
        waitingforBackwordMove(tanksMove, i);
    }
    else{
        printToLogVector[i] = ActionRequestToString(tanksMove); // we know the action 
        bool tankCanMove = canMakeMove(*tanks[i], tanksMove);
        if(!tankCanMove){
            printToLogVector[i] += " (ignored)";
        }

        switch (tanksMove){
            case ActionRequest::RotateLeft45:
            case ActionRequest::RotateRight45:
            case ActionRequest::RotateLeft90:
            case ActionRequest::RotateRight90:
                tanks[i]->setOrientation(calculateNewOrientation(tanksMove, i));
                // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                //         to_string(tanks[i]->getLocation()[1])) + ") turned from " + orientationToString(ornt) + " to " + orientationToString(tanks[i]->getOrientation()) + ".\n", LOG_FILE);
                tanks[i]->setInBackwards(0);
                break;
            case ActionRequest::DoNothing:
                // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                //         to_string(tanks[i]->getLocation()[1])) + ") stayed in place.\n", LOG_FILE);
                tanks[i]->setInBackwards(0);
                break;
            case ActionRequest::MoveForward:
                moveForwardMove(tankCanMove, /* tanksMove,  */i);
                tanks[i]->setInBackwards(0);
                break;
            case ActionRequest::MoveBackward:
                moveBackwardMove(tankCanMove, /* tanksMove,  */i);
                break;
            case ActionRequest::Shoot:
                shootMove(tankCanMove, /* tanksMove,  */i);
                tanks[i]->setInBackwards(0);
                break;
            default:
                break;
        }
        if(tanksMove == ActionRequest::GetBattleInfo){
            tanks[i]->setInBackwards(0);
            OurSattelliteView satellite(*gameBoard, numOfCols , numOfRows, tanks[i]->getLocation()[0], tanks[i]->getLocation()[1]);
            // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
            //             to_string(tanks[i]->getLocation()[1])) + ") requested battle info.\n", LOG_FILE);
            tanksPlayer == 1 ? player1->updateTankWithBattleInfo(*tanks[i]->tankAlg, satellite) : player2->updateTankWithBattleInfo(*tanks[i]->tankAlg, satellite);
        }
    }
}



void gameManager_0000::getMovesFromTanks(){
    unique_ptr<int[]> newLocation = nullptr;
    ActionRequest tanksMove;
    for (size_t i = 0; i < tanks.size(); i++){
        if(!tanks[i]->getIsAlive()) // If the tank is not alive, skip it
        {
            printToLogVector[i] = "killed";
            continue;
        }
        tanks[i]->decrementTurnsFromLastShot();
        tanksMove = tanks[i]->tankAlg->getAction();
        getTheIthTankMove(i, tanksMove);
    }
}

void gameManager_0000::updateAboutNewDstroyedTanks(){
    for(size_t i = 0; i < tanks.size(); ++i){
        if(!tanks[i]->getIsAlive() && tanks[i]->getIsDead() == false){ //if the tank destroyed in this turn
            printToLogVector[i] += " (killed)";
            tanks[i]->setIsDead();

            int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2;
            tanksPlayer == 1 ? numOfP1TanksLeft-- : numOfP2TanksLeft--;
            numOfBulletsLeft -= tanks[i]->getShotsLeft(); // Update the number of bullets left without the destroyed tank's bullets
        }
    }
}


void gameManager_0000::actualymakeMoves(){
    for (size_t i = 0; i < currMovingObjects.size(); i++){ // do the actual move to all the object1 that didn't get destroyed
        if (currMovingObjects[i]->getType() == B && !isSamePoint(currMovingObjects[i]->getOldLocation(), currMovingObjects[i]->getLocation())){ // If a bullet moved (after the first turn)
            // writeToFile("The bullet at (" + (to_string(currMovingObjects[i]->getOldLocation()[0]) + "," + to_string(currMovingObjects[i]->getOldLocation()[1]))
            //          + ") moved to (" + to_string(currMovingObjects[i]->getLocation()[0]) + "," + to_string(currMovingObjects[i]->getLocation()[1]) + ").\n", LOG_FILE);
        }
        int objectNewRow = currMovingObjects[i]->getLocation()[1];
        int objectNewCol = currMovingObjects[i]->getLocation()[0];

        (*gameBoard)[objectNewCol][objectNewRow][2] = nullptr;
        (*gameBoard)[objectNewCol][objectNewRow][1] = currMovingObjects[i];
    }
}


void gameManager_0000::dealWithDamagedUnmovingObject(int i, const int objectNewCol, const int objectNewRow){
    /* int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2; */
    if (checkIfTank(*currMovingObjects[i]) && (*gameBoard)[objectNewCol][objectNewRow][0]->getType() == M){
        // int tankNum = tanks[i]->getTankIndex();
        // if a tank stepped on a mine - they both destroyed
        (*gameBoard)[objectNewCol][objectNewRow][0] = nullptr; // remove the mine from the board
        // int tanksPlayer = currMovingObjects[i]->getType() == P1T ? 1 : 2;
        numOfMinesDestroyed++;
        if (currMovingObjects[i]->getIsAlive()){
            // writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " stepped on a mine at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
            //             to_string(tanks[i]->getLocation()[1])) + ").\n", LOG_FILE);
            currMovingObjects[i]->takeAHit();
        }
        else{
            // writeToFile("The mine at (" + to_string(objectNewCol) + "," + to_string(objectNewCol) + ") has been explode.\n",LOG_FILE);
        }
    }
    else if (currMovingObjects[i]->getType() == B && (*gameBoard)[objectNewCol][objectNewRow][0]->getType() == W){
        // if a bullet hit a wall - the bullet is destroyed and the wall takes a hit

        // writeToFile("A bullet hit a wall at (" + to_string(objectNewCol) + "," + to_string(objectNewRow) +  ") .\n", LOG_FILE);

        currMovingObjects[i]->takeAHit();
        shared_ptr<matrixObject> damagedWall = (*gameBoard)[objectNewCol][objectNewRow][0];
        damagedWall->takeAHit();

        if (!damagedWall->getIsAlive()){ // if the wall destroyed - remove from the board
            (*gameBoard)[objectNewCol][objectNewRow][0] = nullptr;

            // writeToFile("The wall at (" + to_string(objectNewRow) + "," + to_string(objectNewCol) + ") has been destroyed.\n", LOG_FILE);
            numOfWallsDestroyed++;
        }
    }
}

bool gameManager_0000::makeAllMoves(){//return true if the game is over and false otherwise
    updateAboutNewDstroyedTanks();
    for (size_t i = 0; i < currMovingObjects.size(); ++i){
        const int objectNewCol = currMovingObjects[i]->getLocation()[0];
        const int objectNewRow = currMovingObjects[i]->getLocation()[1];

        (*gameBoard)[currMovingObjects[i]->getOldLocation()[0]][currMovingObjects[i]->getOldLocation()[1]][1] = nullptr;
        (*gameBoard)[objectNewCol][objectNewRow][2] = currMovingObjects[i];

        if (!(*gameBoard)[objectNewCol][objectNewRow][0]){}
        else{
            dealWithDamagedUnmovingObject(i, objectNewCol, objectNewRow);
        }
        if (!currMovingObjects[i]->getIsAlive()){
            // Check if the moving object has been destroyed, happened if at least one of the following occur:
            //          - The moving object collides with another moving object
            //          - The moving object is a tank, and it stepped on a mine
            //          - The moving object is a bullet, and it hit a wall

            shared_ptr<matrixObject> destroyedObject = (*gameBoard)[objectNewCol][objectNewRow][2];
            (*gameBoard)[objectNewCol][objectNewRow][2] = nullptr; // remove the object from the board
            currMovingObjects.erase(currMovingObjects.begin() + i);    // remove the object from the moving object1 vector

            if (destroyedObject->getType() == B)
            {
                bullets.erase(
                    std::remove(bullets.begin(), bullets.end(), destroyedObject),
                    bullets.end()); // delete the destroyed bullet from the bullets in the air vector
                    destroyedObject = nullptr;
            }

            --i;
        }
    }
    if (numOfP1TanksLeft == 0 || numOfP2TanksLeft == 0){ // if at least one of the players doesn't have tanks left - the game over
        return true;
    }
    actualymakeMoves(); // do the actual move to all the objects that didn't get destroyed
    return false;
}

bool gameManager_0000::canMakeMove(PseudoTank &tankChoseTheMove, ActionRequest moveChosen){
    unique_ptr<int[]> newLoc;
    if (moveChosen == ActionRequest::MoveForward){
        newLoc = tankChoseTheMove.newLocation(numOfCols, numOfRows);
        if ((*gameBoard)[newLoc[0]][newLoc[1]][0] && (*gameBoard)[newLoc[0]][newLoc[1]][0]->getType() == W){
            return false;
        }
        return true;
    }
    else if (moveChosen == ActionRequest::MoveBackward){
        newLoc = tankChoseTheMove.newLocation(numOfCols, numOfRows , true);
        if ((*gameBoard)[newLoc[0]][newLoc[1]][0] && (*gameBoard)[newLoc[0]][newLoc[1]][0]->getType() == W){
            return false;
        }
    }
    else if (moveChosen == ActionRequest::Shoot){
        if (!tankChoseTheMove.canShoot()){
            return false;
        }
    }
    return true;
}

void gameManager_0000::printSummeryToOurLog(){
    writeToFile("\n", LOG_FILE);
    writeToFile("Game summary:\n", LOG_FILE);
    writeToFile("Turns played: " + to_string(turns) + "\n", LOG_FILE);
    writeToFile("Player number 1 has " + to_string(numOfP1TanksLeft) + " tanks left out of " + to_string(numOfP1Tanks) + ".\n", LOG_FILE);
    writeToFile("Player number 2 has " + to_string(numOfP2TanksLeft) + " tanks left out of " + to_string(numOfP2Tanks) + ".\n", LOG_FILE);
    writeToFile(to_string(numOfWallsDestroyed) + " out of " + to_string(numOfWalls) + " walls were destroyed\n", LOG_FILE);
    writeToFile(to_string(numOfMinesDestroyed) + " out of " + to_string(numOfMines) + " mines were destroyed\n", LOG_FILE);
}

void gameManager_0000::printLastTurnToLog(){
    for(size_t i = 0 ; i < tanks.size() ; ++i){
        writeToFile(printToLogVector[i] , gameMapFileName);
        if( i < printToLogVector.size() - 1 ){
            writeToFile(", ", gameMapFileName);
        }
        printToLogVector[i] = "";
    }
    writeToFile("\n", gameMapFileName);
}


void gameManager_0000::printToOurLogGameResult(){
    printSummeryToOurLog();

    if( numOfP1TanksLeft == 0 && numOfP2TanksLeft == 0){
        writeToFile("Game result: A tie because both players have zero tanks.\n", LOG_FILE);
    }
    else if(numOfP1TanksLeft == 0 || numOfP2TanksLeft == 0){
        int winnerPlayerNum = numOfP2TanksLeft != 0 ? 2 : 1;
        /* int tanksLeftToWinner = numOfP2TanksLeft != 0 ? numOfP2TanksLeft : numOfP1TanksLeft; */
        writeToFile("Game result: Player " + to_string(winnerPlayerNum) + " won.\n", LOG_FILE);
    }
    else if(turns == maxTurns){
        writeToFile("Game result: Tie, reached max steps = " + to_string(maxTurns/2) + ".\n", LOG_FILE);
    }
    else if(numOfBulletsLeft == 0){
        writeToFile("Game result: Tie, both players have zero shells for " + to_string(MAX_STEPS_WITHOUT_SHELLS) + " steps\n", LOG_FILE);
    }
}


void gameManager_0000::printGameResultToLog(){
    
    if( numOfP1TanksLeft == 0 && numOfP2TanksLeft == 0){
        gameResult.winner = 0;
        gameResult.reason = GameResult::ALL_TANKS_DEAD;
        if (verbose)
        {
            writeToFile("Tie, both players have zero tanks", gameMapFileName);
        }
    }
    else if(numOfP1TanksLeft == 0 || numOfP2TanksLeft == 0){
        int winnerPlayerNum = numOfP2TanksLeft != 0 ? 2 : 1;
        gameResult.winner = winnerPlayerNum;
        gameResult.reason = GameResult::ALL_TANKS_DEAD;
        int tanksLeftToWinner = numOfP2TanksLeft != 0 ? numOfP2TanksLeft : numOfP1TanksLeft;
        if (verbose)
        {
            writeToFile("Player " + to_string(winnerPlayerNum) + " won with " + to_string(tanksLeftToWinner) + " tanks still alive\n", gameMapFileName);
        }
        
    }
    else if(turns == maxTurns){
        gameResult.winner = 0;
        gameResult.reason = GameResult::MAX_STEPS;
        if (verbose)
        {
            writeToFile("Tie, reached max steps = " + to_string(maxTurns/2) + 
            ", player 1 has " + to_string(numOfP1TanksLeft) + " tanks, player 2 has " + to_string(numOfP2TanksLeft) + " tanks\n", gameMapFileName);
        }
        
    }
    else if(numOfBulletsLeft == 0){
        gameResult.winner = 0;
        gameResult.reason = GameResult::ZERO_SHELLS;
        if (verbose){
            writeToFile("Tie, both players have zero shells for " + to_string(MAX_STEPS_WITHOUT_SHELLS) + " steps\n", gameMapFileName);
        }
    }

    gameResult.rounds = turns / 2;
    gameResult.remaining_tanks = {size_t(numOfP1TanksLeft), size_t(numOfP2TanksLeft)};
    gameResult.gameState = make_unique<OurSattelliteView>(*gameBoard, numOfCols, numOfRows, -100, -100);
}


GameResult gameManager_0000::run(
    size_t map_width, size_t map_height,
    const SatelliteView& map,
    string map_name,
    size_t max_steps, size_t num_shells,
    Player& player1, string name1, Player& player2, string name2,
    TankAlgorithmFactory player1_tank_algo_factory,
    TankAlgorithmFactory player2_tank_algo_factory)
{
    this->player1 = shared_ptr<Player>(&player1);
    this->player2 = shared_ptr<Player>(&player2);
    this->p1Name = name1;
    this->p2Name = name2;

    this->numOfRows = map_height;
    this->numOfCols = map_width;
    this->gameMapFileName = map_name;
    this->maxTurns = max_steps * 2; // each player has max_steps turns
    this->numOfBulletsLeft = num_shells;
    this->gameOver = false;

    readBoard(map_name, map, player1_tank_algo_factory, player2_tank_algo_factory);

    // writeToFile("\nStarting game\n", LOG_FILE);
    // writeToFile("Player number 1 start with " + to_string(numOfP1TanksLeft) + " tanks.\n", LOG_FILE);
    // writeToFile("Player number 2 start with " + to_string(numOfP2TanksLeft) + " tanks.\n", LOG_FILE);

    while (!gameOver && noBulletsCnt > 0 && turns < maxTurns){
        turns++;
        isOddTurn = !isOddTurn;
        // writeToFile("\n", LOG_FILE);
        // writeToFile("Turn number " + to_string(turns) + ":\n", LOG_FILE);
        if (numOfBulletsLeft == 0){ // if no bullets left, we start to count down the number of turns left until the game is over with a tie
            noBulletsCnt--;
        }
        moveBullets(); // only calculate the bullets new location
        if (isOddTurn){
            getMovesFromTanks();
        }
        gameManager_0000::checkCollisions();
        gameOver = makeAllMoves();
        if(!isOddTurn && verbose){
            printLastTurnToLog();
        }
    }

    printGameResultToLog();
    // printToOurLogGameResult();

    return std::move(gameResult);
}

void gameManager_0000::readBoard(const string &filename, const SatelliteView &map,
    TankAlgorithmFactory tankAlgFactory1, TankAlgorithmFactory tankAlgFactory2){
    try
    {
        filesystem::remove(INP_ERR_FILE);
        filesystem::remove("output_" + filesystem::path(filename).stem().string() + ".txt");
    }
    catch(const std::exception& e)
    {
        // Error in file deletion - not really critical, just continue
    }

    if (!initializeGame(map, tankAlgFactory1, tankAlgFactory2)){
        cerr << "Error: Failed to create map from file, detailes in input_errors.txt." << std::endl;
        exit(EXIT_FAILURE);
    }
    string fileStem = filesystem::path(filename).stem().string();
    gameMapFileName = filesystem::path(filename).replace_extension("").string();
    gameMapFileName.replace((gameMapFileName.length() - fileStem.length()), fileStem.length(), "output_" + fileStem + ".txt");
}

gameManager_0000::gameManager_0000(bool verbose) : verbose(verbose), numOfRows(0), numOfCols(0),
turns(0), noBulletsCnt(2 * MAX_STEPS_WITHOUT_SHELLS), isOddTurn(false), numOfWalls(0), numOfMines(0), numOfWallsDestroyed(0), numOfMinesDestroyed(0), gameBoard(nullptr), tanks(std::vector<std::shared_ptr<PseudoTank>>())
{

}

gameManager_0000::~gameManager_0000() = default;
