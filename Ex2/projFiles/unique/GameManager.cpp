#include "GameManager.h"

#define INP_ERR_FILE "input_errors.txt"
#define LOG_FILE "game_log.txt"
const int MAX_STEPS_WITHOUT_SHELLS = 40;


using namespace std;

bool gameManager::isValidFile(const string &filename) {
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

bool gameManager::parseGameInfo(const string line, const string description, int rowNum) {
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

bool gameManager::getRowsAndColsFromFile(const string &filename){
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
            if (parseGameInfo(line, descriptions[i - 1], i)){
                gotDims = true;
                break;
            }
        }
        else
        {
            writeToFile("Error: Failed to read line from file.\n", INP_ERR_FILE);
            gotDims = false;
        }
        file1.close();
    }

    return gotDims;
}


bool gameManager::addTankToMap(int playerNum, int currCol, int currRow, TankAlgorithmFactory &tankFactory){
    orientation ornt = playerNum == 1 ? L : R;
    objectType type = playerNum == 1 ? P1T : P2T;
    shared_ptr<PseudoTank> tank = make_shared<PseudoTank>(currRow, currCol, type, ornt);
    tank->tankAlg = std::move(tankFactory.create(playerNum, playerNum == 1 ? numOfP1TanksLeft++ : numOfP2TanksLeft++));
    if(!tank->tankAlg){
        writeToFile("Error: Failed to create tank algorithm.\n", INP_ERR_FILE);
        return false;
    }
    printToLogVector.emplace_back(nullptr);

    tanks.push_back(tank);

    currMovingObjects.push_back(tank);

    (*gameBoard)[currCol][currRow][1] = tank;
    return true;
}

void gameManager::addUnmovingObjectToMap(char UnmovingObjectType, int currCol, int currRow){
    if(UnmovingObjectType == '#'){
        (*gameBoard)[currCol][currRow][0] = make_unique<wall>(currRow, currCol, W);
        numOfWalls++;
    }
    else if(UnmovingObjectType == '@'){
        (*gameBoard)[currCol][currRow][0] = make_unique<mine>(currRow, currCol, M);
        numOfMines++;
    }
}



bool gameManager::createMap(const string &filename, TankAlgorithmFactory &tankFactory){
    int currRow = 0, currCol = 0;
    string line;
    ifstream file1;
    file1.open(filename);

    if (!file1.is_open()){
        cerr << "Error: Could not open the file '" << filename << "'." << std::endl;
        return false;
    }
    for(int i = 0 ; i < 4 ; i++){
        getline(file1, line); // Skip the first 5 lines
    }
    while (getline(file1, line)){
        if (currRow == numOfRows){
            writeToFile("Error: Too many rows in the map file.\n", INP_ERR_FILE);
            break;
        }
        for (char ch : line) {
            if (currCol == numOfCols){
                writeToFile("Error: Too many columns in row " + to_string(currRow) + ".\n", INP_ERR_FILE);
                continue;
            }
            switch (ch) {
            case '#':
                addUnmovingObjectToMap('@', currCol, currRow);
                break;
            case '1':
                if(!addTankToMap(1, currCol, currRow, tankFactory)){
                    return false;
                }
                break;
            case '2':
                if(!addTankToMap(2, currCol, currRow, tankFactory)){
                    return false;
                }
                break;
            case '@':
                addUnmovingObjectToMap('@', currCol, currRow);
                break;
            case ' ':
                break;
            default:
                writeToFile("Error: unrecognized character, ASCII #'" + to_string(ch) + "' in the map file.\n", INP_ERR_FILE);
            }
            currCol++;
        }
        if (currCol < numOfCols){
            writeToFile("Error: Not enough columns in row " + to_string(currRow) + ".\n", INP_ERR_FILE);
        }
        currRow++;
        currCol = 0;
    }
    if (currRow < numOfRows){
        writeToFile("Error: Not enough rows in the map file.\n", INP_ERR_FILE);
    }
    file1.close();
    
    return true;
}


// Function initialize the game data from a given pathname.
// If some critical error occurs(criticalErr==true), function fails and returns false(i.e return !criticalErr).
bool gameManager::initializeGame(const string &filename, TankAlgorithmFactory &tankFactory, PlayerFactory &playerFactory){
    gameMapFileName = filename;
    if (gameBoard){
        gameBoard = nullptr;
    }
    if (tanks.empty()){
        tanks.clear();
    }
    if (!isValidFile(filename)){
        return false;
    }
    if (!getRowsAndColsFromFile(filename)){
        return false;
    }

    player1 = playerFactory.create(1, numOfCols, numOfRows, maxTurns, maxBullets);
    player2 = playerFactory.create(2, numOfCols, numOfRows, maxTurns, maxBullets);
    numOfP1TanksLeft = 0, numOfP2TanksLeft = 0;
    gameBoard = std::make_unique<vector<vector<array<shared_ptr<matrixObject>, 3>>>>(
        numOfCols, vector<array<shared_ptr<matrixObject>, 3>>(numOfRows, {nullptr, nullptr, nullptr}));
    if(createMap(filename, tankFactory) == false){
        return false;
    }
    if(numOfP1TanksLeft == 0 || !numOfP2TanksLeft == 0){
        printGameResultToLog();
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

std::string ActionRequestToString(ActionRequest action) {
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

void gameManager::moveBullets(){
    unique_ptr<int[]> newLoc;
    for (shared_ptr<bullet> b : bullets) {
        newLoc = b->newLocation(numOfCols, numOfRows);
        b->setNewLocation(newLoc[0], newLoc[1]);
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

void gameManager::printCollisionsToLog(const movingObject &object1, const movingObject &object2) const{
    if(checkIfTank(object1) && checkIfTank(object2) && (object1.getIsAlive() || object2.getIsAlive())){
        // if at least one of the tanks has been destroyed by a collision between them, and we didn't notified about it
        int object1Player = object1.getType() == P1T ? 1 : 2;
        int object2Player = object2.getType() == P1T ? 1 : 2;
        int object1TankNum = dynamic_cast<const PseudoTank*>(&object1)->getTankNum();
        int object2TankNum = dynamic_cast<const PseudoTank*>(&object2)->getTankNum();
        writeToFile("Tank number " + to_string(object1TankNum) + " of player number " + to_string(object1Player) + " collided with tank number "
         + to_string(object2TankNum) + " of player number " + to_string(object2Player) + " at (" + to_string(object1.getOldLocation()[0]) + "," + to_string(object1.getOldLocation()[1]) + ").\n", LOG_FILE);
    }
    if (object1.getIsAlive() && object2.getIsAlive()){// we need to notify that the two objects been destroyed
        if (checkIfTank(object1) && object2.getType() == B){
            int tankPlayer = object1.getType() == P1T ? 1 : 2;
            int objectTankNum = dynamic_cast<const PseudoTank*>(&object1)->getTankNum();
            writeToFile("Tank number " + to_string(objectTankNum) + " of player number " + to_string(tankPlayer) 
                + " got hit by a bullet at (" + to_string(object1.getOldLocation()[0]) + "," + to_string(object1.getOldLocation()[1]) + ").\n", LOG_FILE);
        }
        else if(object1.getType() == B && checkIfTank(object2)){
            int tankPlayer = object2.getType() == P1T ? 1 : 2;
            int objectTankNum = dynamic_cast<const PseudoTank*>(&object2)->getTankNum();
            writeToFile("Tank number " + to_string(objectTankNum) + " of player number " + to_string(tankPlayer) 
                + " got hit by a bullet at " + to_string(object1.getOldLocation()[0]) + "," + to_string(object1.getOldLocation()[1]) + ".\n", LOG_FILE);
        }
        else if (object1.getType() == B && object2.getType() == B){
            writeToFile("The bullet at (" + (to_string(object1.getOldLocation()[0]) + "," + (to_string(object1.getOldLocation()[1])) + ") collided with the bullet at (" +
                         to_string(object2.getOldLocation()[0])) + "," + (to_string(object2.getOldLocation()[1])) + ") destroying each other.\n", LOG_FILE);
        }
    }
    else if (!object1.getIsAlive() && !object2.getIsAlive()){
        //we already notified that the two objects been destroyed thus we don't need to notify again
    }
    else if ((object1.getIsAlive() && object1.getType() == B) || (object2.getIsAlive() && object2.getType() == B)){// we need to notify that an object been destroyed
            writeToFile("A bullet has been destroyed by collision at (" + to_string(object1.getOldLocation()[0]) + "," + to_string(object1.getOldLocation()[1]) + ").\n", LOG_FILE);
    }
}

// This function was written with the help of chat gpt
// Function to check for collisions between moving object1
bool gameManager::checkCollisions(){
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
                    //printCollisionsToLog(*currMovingObjects[i], *currMovingObjects[j]);
                    currMovingObjects[i]->takeAHit();
                    currMovingObjects[j]->takeAHit();
                }
            }
        }
    }

    return true;
}

 
void gameManager::waitingforBackwordMove(ActionRequest tanksMove, int i){
    unique_ptr<int[]> tankNewLocation = nullptr;
    int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2;
    int tankNum = tanks[i]->getTankNum();
    if(tanksMove == ActionRequest::MoveForward) {
        tanks[i]->setInBackwards(0);
        writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at ()" + (to_string(tanks[i]->getLocation()[0]) + "," +
                                      to_string(tanks[i]->getLocation()[1])) + ") stayed in place.\n", LOG_FILE);

        printToLogVector[i] = "MoveForward";
    }
    else {
        if (tanksMove != ActionRequest::DoNothing){
            writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                                      to_string(tanks[i]->getLocation()[1])) + ") tried to make a move while in backwards mode.\n", LOG_FILE);
            printToLogVector[i] = ActionRequestToString(tanksMove) + " (ignored)";
        }
        else { // If the tank is in backwards mode and tries to do nothing, we just increase the inBackwards counter
            tanks[i]->setInBackwards(tanks[i]->getInBack() + 1);
            printToLogVector[i] = ActionRequestToString(tanksMove);
            if(tanks[i]->getInBack() == 3){
                tankNewLocation = tanks[i]->newLocation(numOfCols, numOfRows, true);
                tanks[i]->setNewLocation(tankNewLocation[0], tankNewLocation[1]);
                writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + to_string(tanks[i]->getOldLocation()[0]) + "," + to_string(tanks[i]->getOldLocation()[1])
                             + ") moved backwards to (" + to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]) + ").\n", LOG_FILE);
            }
            else{
                writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                                      to_string(tanks[i]->getLocation()[1])) + ") stayed in place waiting until he can move backwards.\n", LOG_FILE);
            }
        }
    }
}

void gameManager::moveForwardMove(bool tankCanMove ,/*ActionRequest tanksMove,*/ int i){
    int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2;
    int tankNum = tanks[i]->getTankNum();
    unique_ptr<int[]> tankNewLocation = nullptr;
    tanks[i]->setInBackwards(0);
    if (tankCanMove){
            tankNewLocation = tanks[i]->newLocation(numOfCols, numOfRows);
            tanks[i]->setNewLocation(tankNewLocation[0], tankNewLocation[1]);
            writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + to_string(tanks[i]->getOldLocation()[0]) + "," + to_string(tanks[i]->getOldLocation()[1])
                            + ") went forwards to (" + to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]) + ").\n", LOG_FILE);
    }
    else{
        writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                                to_string(tanks[i]->getLocation()[1])) + ") tried to move forwards when he couldn't (unstoppable force met an immovable object).\n", LOG_FILE);
    }
}

void gameManager::moveBackwardMove(bool tankCanMove ,/* ActionRequest tanksMove,  */int i){
    int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2;
    int tankNum = tanks[i]->getTankNum();
    unique_ptr<int[]> tankNewLocation = nullptr;
    if (tankCanMove){
        if (tanks[i]->getInBack() >= 3){ // if we have moved backwards last turn and want to move
            tanks[i]->setInBackwards(tanks[i]->getInBack() + 1);
            tankNewLocation = tanks[i]->newLocation(numOfCols, numOfRows, true);
            tanks[i]->setNewLocation(tankNewLocation[0], tankNewLocation[1]);
            writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + to_string(tanks[i]->getOldLocation()[0]) + "," + to_string(tanks[i]->getOldLocation()[1])
                            + ") moved backwards to (" + to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]) + ").\n", LOG_FILE);
        }
        else{
            tanks[i]->setInBackwards(1);
               writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                                      to_string(tanks[i]->getLocation()[1])) + ") stayed in place waiting until he can move backwards.\n", LOG_FILE);
        }
    }
    else{
        writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                                to_string(tanks[i]->getLocation()[1])) + ") tried to move backwards when he couldn't (unstoppable force met an immovable object).\n", LOG_FILE);
        tanks[i]->setInBackwards(0);
    }
}

void gameManager::shootMove(bool tankCanMove,/*  ActionRequest tanksMove,  */int i){
    int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2;
    int tankNum = tanks[i]->getTankNum();
    unique_ptr<int[]> newBulletLocation = nullptr;
    if (tankCanMove){
        newBulletLocation = tanks[i]->newLocation(numOfCols, numOfRows);               // Get bullet location
        shared_ptr<bullet> b = make_unique<bullet>(newBulletLocation[1], newBulletLocation[0], tanks[i]->getOrientation(), B); // Create bullet
        currMovingObjects.emplace_back(b);
        bullets.emplace_back(b);
        tanks[i]->useShot();
        writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                        to_string(tanks[i]->getLocation()[1])) + ") shot a bullet.\n", LOG_FILE);
        writeToFile("The new bullet starting location is (" + (to_string(b->getLocation()[0]) + "," + to_string(b->getLocation()[1])) + ").\n", LOG_FILE);
    }
    else{
        writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                        to_string(tanks[i]->getLocation()[1])) + ") shot a blank.\n", LOG_FILE);
    }
}


orientation gameManager::calculateNewOrientation(ActionRequest &tanksMove, int i){
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


void gameManager::getTheIthTankMove(int i, ActionRequest &tanksMove){
    int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2;
    int tankNum = tanks[i]->getTankNum();
    orientation ornt = tanks[i]->getOrientation();
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
                writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                        to_string(tanks[i]->getLocation()[1])) + ") turned from " + orientationToString(ornt) + " to " + orientationToString(tanks[i]->getOrientation()) + ".\n", LOG_FILE);
                break;
            case ActionRequest::DoNothing:
                writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                        to_string(tanks[i]->getLocation()[1])) + ") stayed in place.\n", LOG_FILE);
                tanks[i]->setInBackwards(0);
                break;
            case ActionRequest::MoveForward:
                moveForwardMove(tankCanMove, /* tanksMove,  */i);
                break;
            case ActionRequest::MoveBackward:
                moveBackwardMove(tankCanMove, /* tanksMove,  */i);
                break;
            case ActionRequest::Shoot:
                shootMove(tankCanMove, /* tanksMove,  */i);
                break;
            default:
                break;
        }
        if(tanksMove == ActionRequest::GetBattleInfo){
            OurSattelliteView satellite(*gameBoard, numOfCols , numOfRows, tanks[i]->getLocation()[0], tanks[i]->getLocation()[1]);
            writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                        to_string(tanks[i]->getLocation()[1])) + ") requested battle info.\n", LOG_FILE);
            tanksPlayer == 1 ? player1->updateTankWithBattleInfo(*tanks[i]->tankAlg, satellite) : player2->updateTankWithBattleInfo(*tanks[i]->tankAlg, satellite);
        }
    }
}



void gameManager::getMovesFromTanks(){
    unique_ptr<int[]> newLocation = nullptr;
    ActionRequest tanksMove;
    for (size_t i = 0; i < tanks.size(); i++){
        if(!tanks[i]->getIsAlive()) // If the tank is not alive, skip it
        {
            printToLogVector[i] = "killed";
            continue;
        }
        tanks[i]->incrementTurnsFromLastShot();
        tanksMove = tanks[i]->tankAlg->getAction();
        getTheIthTankMove(i, tanksMove);

        tanks[i]->setNewLocation(tanks[i]->getLocation()[0], tanks[i]->getLocation()[1]);
    }
}

void gameManager::updateAboutNewDstroyedTanks(){
    for(size_t i = 0; i < tanks.size(); ++i){
        if(!tanks[i]->getIsAlive() && printToLogVector[i] != "killed"){ //if the tank destroyed in this turn
            printToLogVector[i] += " (killed)";

            int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2;
            tanksPlayer == 1 ? numOfP1TanksLeft-- : numOfP2TanksLeft--;
            numOfBulletsLeft -= tanks[i]->getShotsLeft(); // Update the number of bullets left without the destroyed tank's bullets
        }
    }
}


void gameManager::actualymakeMoves(){
    for (size_t i = 0; i < currMovingObjects.size(); ++i){ // do the actual move to all the object1 that didn't get destroyed
        if (currMovingObjects[i]->getType() == B && !isSamePoint(currMovingObjects[i]->getOldLocation(), currMovingObjects[i]->getLocation())){ // If a bullet moved (after the first turn)
            writeToFile("The bullet at (" + (to_string(currMovingObjects[i]->getOldLocation()[0]) + "," + to_string(currMovingObjects[i]->getOldLocation()[1]))
                     + ") moved to (" + to_string(currMovingObjects[i]->getLocation()[0]) + "," + to_string(currMovingObjects[i]->getLocation()[1]) + ").\n", LOG_FILE);
        }
        int objectNewRow = currMovingObjects[i]->getLocation()[1];
        int objectNewCol = currMovingObjects[i]->getLocation()[0];

        (*gameBoard)[objectNewCol][objectNewRow][2] = nullptr;
        (*gameBoard)[objectNewCol][objectNewRow][1] = currMovingObjects[i];
    }
}


void gameManager::dealWithDamagedUnmovingObject(int i, const int objectNewCol, const int objectNewRow){
    /* int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2; */
    int tankNum = tanks[i]->getTankNum();
    if (checkIfTank(*currMovingObjects[i]) && (*gameBoard)[objectNewCol][objectNewRow][0]->getType() == M){
        // if a tank stepped on a mine - they both destroyed
        (*gameBoard)[objectNewCol][objectNewRow][0] = nullptr; // remove the mine from the board
        int tanksPlayer = currMovingObjects[i]->getType() == P1T ? 1 : 2;

        if (currMovingObjects[i]->getIsAlive()){
            writeToFile("Tank number " + to_string(tankNum) + " of player number " + to_string(tanksPlayer) + " stepped on a mine at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                        to_string(tanks[i]->getLocation()[1])) + ") requested battle info.\n", LOG_FILE);
            currMovingObjects[i]->takeAHit();
        }
        else{
            writeToFile("The mine at (" + to_string(objectNewCol) + "," + to_string(objectNewCol) + ") has been explode.\n",LOG_FILE);
            numOfMinesDestroyed++;
        }
    }
    else if (currMovingObjects[i]->getType() == B && (*gameBoard)[objectNewCol][objectNewRow][0]->getType() == W){
        // if a bullet hit a wall - the bullet is destroyed and the wall takes a hit

        writeToFile("A bullet hit a wall at (" + to_string(objectNewRow) + "," + to_string(objectNewCol) +  ") .\n", LOG_FILE);

        currMovingObjects[i]->takeAHit();
        shared_ptr<matrixObject> damagedWall = (*gameBoard)[objectNewCol][objectNewRow][0];
        damagedWall->takeAHit();

        if (!damagedWall->getIsAlive()){ // if the wall destroyed - remove from the board
            (*gameBoard)[objectNewCol][objectNewRow][0] = nullptr;

            writeToFile("The wall at (" + to_string(objectNewRow) + "," + to_string(objectNewCol) + ") has been destroyed.\n", LOG_FILE);
            numOfWallsDestroyed++;
        }
    }
}

bool gameManager::makeAllMoves(){//return true if the game is over and false otherwise
    for (size_t i = 0; i < currMovingObjects.size(); ++i){
        const int objectNewCol = currMovingObjects[i]->getLocation()[0];
        const int objectNewRow = currMovingObjects[i]->getLocation()[1];

        (*gameBoard)[currMovingObjects[i]->getOldLocation()[0]][currMovingObjects[i]->getOldLocation()[1]][1] = nullptr;
        (*gameBoard)[objectNewCol][objectNewRow][2] = currMovingObjects[i];

        if (!(*gameBoard)[objectNewCol][objectNewRow][0]){}
        else{
            dealWithDamagedUnmovingObject(i, objectNewCol, objectNewRow);
        }
        updateAboutNewDstroyedTanks();
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

bool gameManager::canMakeMove(PseudoTank &tankChoseTheMove, ActionRequest moveChosen)
{
    unique_ptr<int[]> newLoc;
    if (moveChosen == ActionRequest::MoveForward)
    {
        newLoc = tankChoseTheMove.newLocation(numOfCols, numOfRows);
        if ((*gameBoard)[newLoc[0]][newLoc[1]][0] && (*gameBoard)[newLoc[0]][newLoc[1]][0]->getType() == W)
        {
            return false;
        }
        return true;
    }
    else if (moveChosen == ActionRequest::MoveBackward)
    {
        newLoc = tankChoseTheMove.newLocation(numOfCols, numOfRows , true);
        if ((*gameBoard)[newLoc[0]][newLoc[1]][0] && (*gameBoard)[newLoc[0]][newLoc[1]][0]->getType() == W)
        {
            return false;
        }
    }
    else if (moveChosen == ActionRequest::Shoot)
    {
        if (!tankChoseTheMove.canShoot())
        {
            return false;
        }
    }
    return true;
}

void gameManager::printSummeryToOurLog(){
    writeToFile("\n", LOG_FILE);
    writeToFile("Game summary:\n", LOG_FILE);
    writeToFile("Turns played: " + to_string(turns) + "\n", LOG_FILE);
    writeToFile("Player number 1 has " + to_string(numOfP1TanksLeft) + " tanks left out of " + to_string(numOfP1Tanks) + ".\n", LOG_FILE);
    writeToFile("Player number 2 has " + to_string(numOfP2TanksLeft) + " tanks left out of " + to_string(numOfP2Tanks) + ".\n", LOG_FILE);
    writeToFile(to_string(numOfWallsDestroyed) + " out of " + to_string(numOfWalls) + " walls were destroyed\n", LOG_FILE);
    writeToFile(to_string(numOfMinesDestroyed) + " out of " + to_string(numOfMines) + " mines were destroyed\n", LOG_FILE);
}

void gameManager::printLastTurnToLog()
{
    for(size_t i = 0 ; i < printToLogVector.size() ; ++i){
        writeToFile(printToLogVector[i] , gameMapFileName);
        if( i < printToLogVector.size() - 1 ){
            writeToFile(", ", gameMapFileName);
        }
    }
    writeToFile("\n", gameMapFileName);
}


void gameManager::printToOurLogGameResult(){
    printSummeryToOurLog();

    if( numOfP1TanksLeft == 0 && numOfP2TanksLeft == 0){
        writeToFile("Game result: A tie because both players have zero tanks.\n", LOG_FILE);
    }
    else if(numOfP1TanksLeft == 0 || numOfP2TanksLeft != 0){
        int winnerPlayerNum = numOfP2TanksLeft != 0 ? 2 : 1;
        /* int tanksLeftToWinner = numOfP2TanksLeft != 0 ? numOfP2TanksLeft : numOfP1TanksLeft; */
        writeToFile("Game result: Player " + to_string(winnerPlayerNum) + " won.\n", LOG_FILE);
    }
    else if(turns == maxTurns){
        writeToFile("Game result: Tie, reached max steps =" + to_string(maxTurns) + ".\n", LOG_FILE);
    }
    else if(numOfBulletsLeft == 0){
        writeToFile("Game result: Tie, both players have zero shells for " + to_string(MAX_STEPS_WITHOUT_SHELLS) + " steps\n", LOG_FILE);
    }
}


void gameManager::printGameResultToLog(){
    // printSummeryToLog();

    if( numOfP1TanksLeft == 0 && numOfP2TanksLeft == 0){
        writeToFile("Tie, both players have zero tanks", gameMapFileName);
    }
    else if(numOfP1TanksLeft == 0 || numOfP2TanksLeft != 0){
        int winnerPlayerNum = numOfP2TanksLeft != 0 ? 2 : 1;
        int tanksLeftToWinner = numOfP2TanksLeft != 0 ? numOfP2TanksLeft : numOfP1TanksLeft;
        writeToFile("Player " + to_string(winnerPlayerNum) + " won with " + to_string(tanksLeftToWinner) + " tanks still alive\n", gameMapFileName);
    }
    else if(turns == maxTurns){
        writeToFile("Tie, reached max steps =" + to_string(maxTurns) + ", player 1 has " + to_string(numOfP1TanksLeft) + " tanks, player 2 has " + to_string(numOfP2TanksLeft) + " tanks\n", gameMapFileName);
    }
    else if(numOfBulletsLeft == 0){
        writeToFile("Tie, both players have zero shells for " + to_string(MAX_STEPS_WITHOUT_SHELLS) + " steps\n", gameMapFileName);
    }
}


void gameManager::run()
{
    writeToFile("Starting game\n", LOG_FILE);
    writeToFile("Player number 1 start with " + to_string(numOfP1TanksLeft) + " tanks.\n", LOG_FILE);
    writeToFile("Player number 2 start with " + to_string(numOfP2TanksLeft) + " tanks.\n", LOG_FILE);
    bool gameOver = false;

    while (!gameOver && noBulletsCnt > 0 && turns < maxTurns){
        turns++;
        isOddTurn = !isOddTurn;
        writeToFile("\n", LOG_FILE);
        writeToFile("Turn number " + to_string(turns) + ":\n", LOG_FILE);
        if (numOfBulletsLeft == 0){ // if no bullets left, we start to count down the number of turns left until the game is over with a tie
            noBulletsCnt--;
        }
        moveBullets(); // only calculate the bullets new location
        if (isOddTurn){
            getMovesFromTanks();
        }
        gameManager::checkCollisions();
        gameOver = makeAllMoves();
        printLastTurnToLog();
    }
    printGameResultToLog();
    printToOurLogGameResult();
    // destroyBoardAndObjects();
}

void gameManager::readBoard(const string &filename){
    try
    {
        filesystem::remove(INP_ERR_FILE);
        filesystem::remove("output_" + filesystem::path(filename).stem().string() + ".txt");
    }
    catch(const std::exception& e)
    {
        // Error in file deletion - not really critical, just continue
    }
    
    if (!initializeGame(filename, tankAlgFactory, playersFactory)){
        cerr << "Error: Failed to create map from file, detailes in input_errors.txt." << std::endl;
        exit(EXIT_FAILURE);
    }
    string fileStem = filesystem::path(filename).stem().string();
    gameMapFileName = filesystem::path(filename).replace_extension("").string();
    gameMapFileName.replace((gameMapFileName.length() - fileStem.length()), fileStem.length(), "output_" + fileStem + ".txt");
}

gameManager::gameManager(TankAlgorithmFactory &tankFactory, PlayerFactory &playerFactory) :  tankAlgFactory(tankFactory), playersFactory(playerFactory), numOfRows(0), numOfCols(0),
turns(0), noBulletsCnt(MAX_STEPS_WITHOUT_SHELLS), isOddTurn(false), numOfWalls(0), numOfMines(0), numOfWallsDestroyed(0), numOfMinesDestroyed(0), gameBoard(nullptr), tanks(vector<shared_ptr<PseudoTank>>{})
{
}

gameManager::~gameManager() = default;

/*
 * # - wall
 * ' ' - empty space
 * 1 - player1 tank
 * 2 - player2 tank
 * @ - mine
 *
 * No bullets at the start of the game
 */

// ```
//
// ### Explanation:
// 1. **Point and MovingObject Structures**: Represent points and moving object1 with start and end positions.
// 2. **Line Intersection Check**: The `doLinesIntersect` function uses orientation and collinearity checks to determine if two line segments intersect.
// 3. **Collision Checker**: The `checkCollisions` function iterates through all pairs of moving object1 and checks for intersections.
//
// You can use this function by passing a list of moving object1 with their planned movements.
