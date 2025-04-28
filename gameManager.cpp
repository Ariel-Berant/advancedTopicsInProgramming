#include "gameManager.h"

#define INP_ERR_FILE "input_errors.txt"

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

bool gameManager::getRowsAndColsFromFile(const string &filename) {
    int cols, rows;
    string line;
    bool gotDims = true;

    ifstream file1;
    file1.open(filename);
    if (!file1.is_open())
    {
        writeToFile("Error: Could not open the file '" + filename + "'.\n", INP_ERR_FILE);
        return false;
    }

    if (getline(file1, line))
    {
        istringstream iss(line);
        if (iss >> rows >> cols)
        {
            numOfCols = cols;
            numOfRows = rows;
        }
        else
        {
            writeToFile("Error: Failed to parse dimensions from line.\n", INP_ERR_FILE);
            gotDims = false;
        }
    }
    else
    {
        writeToFile("Error: Failed to read line from file.\n", INP_ERR_FILE);
        gotDims = false;
    }
    file1.close();

    return gotDims;
}

// Function creates map from a given pathname.
// If some critical error occurs(criticalErr==true), function fails and returns false(i.e return !criticalErr).
bool gameManager::createMap(const string &filename)
{
    if (!isValidFile(filename))
    {
        return false;
    }

    if (!getRowsAndColsFromFile(filename))
    {
        return false;
    }
    
    ifstream file1;
    file1.open(filename);
    if (!file1.is_open())
    {
        cerr << "Error: Could not open the file '" << filename << "'." << endl;
        return false;
    }

    int currRow = 0, currCol = 0;
    string line;
    bool tank1appeared = false , tank2appeared = false;

    gameBoard = new vector<vector<array<matrixObject *, 3>>>(numOfRows, vector<array<matrixObject *, 3>>(numOfCols));

    getline(file1, line); // Skip the first line with the dimensions

    while (getline(file1, line))
    {
        if (currRow == numOfRows)
        {
            writeToFile("Error: Too many rows in the map file.\n", INP_ERR_FILE);
            break;
        }
        for (char ch : line)
        {
            if (currCol == numOfCols)
            {
                writeToFile("Error: Too many columns in row " + to_string(currRow) + ".\n", INP_ERR_FILE);
                continue;
            }
            switch (ch)
            {
            case '#':
                (*gameBoard)[currRow][currCol][0] = new wall(currRow, currCol, W);
                numOfWalls++;
                (*gameBoard)[currRow][currCol][1] = nullptr;
                (*gameBoard)[currRow][currCol][2] = nullptr;
                break;
            case '1':
                if (!tanks[0])
                {
                    tanks[0] = new p1Tank(currRow, currCol, L);
                    (*gameBoard)[currRow][currCol][1] = tanks[0];
                    tank1appeared = true;
                }
                else
                {
                    writeToFile("Error: More than one tank for player 1.\n", INP_ERR_FILE);
                    (*gameBoard)[currRow][currCol][1] = nullptr;
                }
                (*gameBoard)[currRow][currCol][0] = nullptr;
                (*gameBoard)[currRow][currCol][2] = nullptr;
                break;
            case '2':
                if (!tanks[1])
                {
                    tanks[1] = new p2Tank(currRow, currCol, R);
                    (*gameBoard)[currRow][currCol][1] = tanks[1];
                    tank2appeared = true;
                }
                else
                {
                    writeToFile("Error: More than one tank for player 2.\n", INP_ERR_FILE);
                    (*gameBoard)[currRow][currCol][1] = nullptr;
                }
                (*gameBoard)[currRow][currCol][0] = nullptr;
                (*gameBoard)[currRow][currCol][2] = nullptr;
                break;
            case '@':
                (*gameBoard)[currRow][currCol][0] = new mine(currRow, currCol, M);
                numOfMines++;
                (*gameBoard)[currRow][currCol][1] = nullptr;
                (*gameBoard)[currRow][currCol][2] = nullptr;
                break;
            case ' ':
                (*gameBoard)[currRow][currCol][0] = nullptr;
                (*gameBoard)[currRow][currCol][1] = nullptr;
                (*gameBoard)[currRow][currCol][2] = nullptr;
                break;
            default:
                writeToFile("Error: unrecognized character, ASCII #'" + to_string(ch)
                + "' in the map file.\n", INP_ERR_FILE);
                (*gameBoard)[currRow][currCol][0] = nullptr;
                (*gameBoard)[currRow][currCol][1] = nullptr;
                (*gameBoard)[currRow][currCol][2] = nullptr;
            }
            currCol++;
        }
        if (currCol < numOfCols)
        {
            writeToFile("Error: Not enough columns in row " + to_string(currRow) + ".\n", INP_ERR_FILE);
        }
        while (currCol < numOfCols)
        {
            (*gameBoard)[currRow][currCol][0] = nullptr;
        }
        currRow++;
        currCol = 0;
    }
    if(!tank1appeared || !tank2appeared){
        writeToFile("Error: There are missing tanks in the map file.\n", INP_ERR_FILE);
        file1.close();
        return false;
    }
    if (currRow < numOfRows)
    {
        writeToFile("Error: Not enough rows in the map file.\n", INP_ERR_FILE);
    }
    while (currRow < numOfRows)
    {
        for (int i = 0; i < numOfCols; ++i)
        {
            (*gameBoard)[currRow][i][0] = nullptr;
        }
        currRow++;
    }

    file1.close();
    return true;
}

void gameManager::moveBullets()
{
    int *newLoc;
    for (bullet *b : bullets){
        newLoc = b->newLocation(numOfCols, numOfRows);
        b->setNewLocation(newLoc[0], newLoc[1]);
        delete[] newLoc;
        newLoc = nullptr;
    }
}

// Helper: Check if two points are the same
bool isSamePoint(const int *a, const int *b)
{
    return a[0] == b[0] && a[1] == b[1];
}

// The next two functions were written with the help of ChatGPT

// Helper: Check if three points are collinear
bool areCollinear(const int *p, const int *q, const int *r)
{
    return (q[1] - p[1]) * (r[0] - q[0]) == (q[0] - p[0]) * (r[1] - q[1]);
}

bool passingBy(const int *aStart, const int *aEnd, const int *bStart, const int *bEnd)
{
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

void gameManager::printCollisionsToLog(const movingObject &object1, const movingObject &object2) const
{
    if(checkIfTank(object1) && checkIfTank(object2) && (object1.getIsAlive() || object2.getIsAlive())){
        // if at least one of the tanks has been destroyed by a collision between them, and we didn't notified about it
        writeToFile("A tank of player number 1 collided with a tank of player number 2.\n", gameMapFileName);
    }
    if (object1.getIsAlive() && object2.getIsAlive()){// we need to notify that the two objects been destroyed
        if ((checkIfTank(object1) && object2.getType() == B) || (object1.getType() == B && checkIfTank(object2))){
            int tanks1Player = object1.getType() == P1T ? 1 : 2;
            writeToFile("A tank of player number " + to_string(tanks1Player) + " got hit by a bullet.\n", gameMapFileName);
        }
        else if (object1.getType() == B && object2.getType() == B){
            writeToFile("The bullet at (" + (to_string(object1.getOldLocation()[0]) + "," + (to_string(object1.getOldLocation()[1])) + ") collided with the bullet at (" +
                         to_string(object2.getOldLocation()[0])) + "," + (to_string(object2.getOldLocation()[1])) + ") destroying each other.\n", gameMapFileName);
        }
    }
    else if (!object1.getIsAlive() && !object2.getIsAlive()){
        //we already notified that the two objects been destroyed thus we don't need to notify again
    }
    else if ((object1.getIsAlive() && object1.getType() == B) || (object2.getIsAlive() && object2.getType() == B)){// we need to notify that an object been destroyed
            writeToFile("A bullet has been destroyed by collision.\n", gameMapFileName);
    }
}

// This function was written with the help of chat gpt
// Function to check for collisions between moving object1
bool gameManager::checkCollisions()
{
    for (size_t i = 0; i < currMovingObjects.size(); i++)
    {
        for (size_t j = i + 1; j < currMovingObjects.size(); j++)
        {
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

void gameManager::makeTankMoves()
{
    orientation ornt;
    objMove tanksMove;
    int *newLocation = nullptr;
//    right now only two tanksArr are supported, but in future will be i < tanksArr.size(), and tanksArr[i].play() will be
//    tanksArr[i].play(gameBoard, {tanksArr without i-th member}, numOfCols, numOfRows);
    for (int i = 0; i < 2; i++)
    {
        tanks[i]->updateTurn();
        tanksMove = tanks[i]->play(*gameBoard, tanks[1 - i]->getLocation(), numOfCols, numOfRows);
        int tanksPlayer = tanks[i]->getType() == P1T ? 1 : 2;

        if (tanks[i]->getInBack() > 0 && tanks[i]->getInBack() < 3)
        {
            if(tanksMove == moveForward) {
                tanks[i]->setInBackwards(0);
                writeToFile("The tank of player " + to_string(tanksPlayer) + " at " + (to_string(tanks[i]->getLocation()[0]) + "," +
                                              to_string(tanks[i]->getLocation()[1]))
                            + " stayed in place.\n", gameMapFileName);
            }
            else {
                if (tanksMove != noAction) {
                    writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                                                  to_string(tanks[i]->getLocation()[1]))
                                + ") tried to make a move while in backwards mode.\n", gameMapFileName);
                }
                else {
                    tanks[i]->setInBackwards(tanks[i]->getInBack() + 1);
                    if(tanks[i]->getInBack() == 3){
                        newLocation = tanks[i]->newLocation(numOfCols, numOfRows , true);
                        tanks[i]->setNewLocation(newLocation[0], newLocation[1]);
                        writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + to_string(tanks[i]->getOldLocation()[0]) + "," + to_string(tanks[i]->getOldLocation()[1])
                                    + ") moved backwards to (" + to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]) + ").\n", gameMapFileName);
                    }
                    else{
                        writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," +
                                                      to_string(tanks[i]->getLocation()[1]))
                                    + ") stayed in place waiting until he can move backwards.\n", gameMapFileName);
                    }
                }
            }
        }

        else
        {
            switch (tanksMove)
            {
            case rotateEighthLeft:
            case rotateEighthRight:
            case rotateQuarterLeft:
            case rotateQuarterRight:
                ornt = tanks[i]->getOrientation();
                tanks[i]->setOrientation(orientation((8 + tanks[i]->getOrientation() - 5 + tanksMove) % 8));
                writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]))
                            + ") turned " + to_string(45 * abs(ornt - tanks[i]->getOrientation())) + " degrees"
                            + (tanksMove<5 ? " clockwise.\n":" counter-clockwise.\n"), gameMapFileName);
                            tanks[i]->setInBackwards(0);
                break;
            case noAction:
                writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]))
                + ") stayed in place.\n", gameMapFileName);
                tanks[i]->setInBackwards(0);
                break;
            case moveForward:
            tanks[i]->setInBackwards(0);
                if (canMakeMove(*tanks[i], tanksMove))
                {
                    newLocation = tanks[i]->newLocation(numOfCols, numOfRows);
                    tanks[i]->setNewLocation(newLocation[0], newLocation[1]);
                    writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getOldLocation()[0]) + "," + to_string(tanks[i]->getOldLocation()[1]))
                    + ") went forwards to (" + to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]) + ").\n", gameMapFileName);
                }
                else
                {
                    writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]))
                    + ") tried to move forwards when he couldn't (unstoppable force met an immovable object).\n", gameMapFileName);
                }
                break;
            case moveBackwards:
                if (canMakeMove(*tanks[i], tanksMove))
                {
                    if (tanks[i]->getInBack() >= 3) // if we have moved backwards last turn and want to move
                    {
                        tanks[i]->setInBackwards(tanks[i]->getInBack() + 1);
                        newLocation = tanks[i]->newLocation(numOfCols, numOfRows , true);
                        tanks[i]->setNewLocation(newLocation[0], newLocation[1]);
                        writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]))
                        + ") moved backwards to (" + to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]) + ").\n", gameMapFileName);
                    }
                    else
                    {
                        tanks[i]->setInBackwards(1);
                        writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]))
                        + ") stayed in place waiting until he can move backwards.\n", gameMapFileName);
                    }
                }
                else
                {
                    writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]))
                    + ") tried to move backwards when he couldn't (unstoppable force met an immovable object).\n", gameMapFileName);
                    tanks[i]->setInBackwards(0);
                }
                break;
            case shoot:
                if (canMakeMove(*tanks[i], tanksMove))
                {
                    newLocation = tanks[i]->newLocation(numOfCols, numOfRows);               // Get bullet location
                    bullet *b = new bullet(newLocation[0], newLocation[1], tanks[i]->getOrientation(), B); // Create bullet
                    currMovingObjects.push_back(b);
                    bullets.push_back(b);
                    tanks[i]->useShot();
                    delete[] newLocation;
                    newLocation = nullptr; // Set to nullptr to enter the else statement(at 430)
                    writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanks[i]->getLocation()[0]) + "," + to_string(tanks[i]->getLocation()[1]))
                        + ") shot a bullet.\n", gameMapFileName);
                    writeToFile("The new bullet starting location is (" + (to_string(b->getLocation()[0]) + "," + to_string(b->getLocation()[1])) + ").\n", gameMapFileName);
                }
                else
                {
                    writeToFile("The tank of player number " + to_string(tanks[i]->getType() == P1T ? 1 : 2) + " shot a blank.\n", gameMapFileName);
                }

            default:
                break;
            }
        }

        if (newLocation != nullptr)
        {
            delete[] newLocation; // Free the memory allocated for newLocation
            newLocation = nullptr; // Set it to nullptr to avoid dangling pointer
        }
        else
        {
            tanks[i]->setNewLocation(tanks[i]->getLocation()[0], tanks[i]->getLocation()[1]);
        }
    }
}


bool gameManager::makeAllMoves()//return true if the game is over and false otherwise
{
    for (size_t i = 0; i < currMovingObjects.size(); ++i)
    {

        const int objectNewRow = currMovingObjects[i]->getLocation()[0];
        const int objectNewCol = currMovingObjects[i]->getLocation()[1];

        (*gameBoard)[currMovingObjects[i]->getOldLocation()[0]][currMovingObjects[i]->getOldLocation()[1]][1] = nullptr;
        (*gameBoard)[objectNewRow][objectNewCol][2] = currMovingObjects[i];

        if (!(*gameBoard)[objectNewRow][objectNewCol][0]){}
        else if ((currMovingObjects[i]->getType() == P1T || currMovingObjects[i]->getType() == P2T) && (*gameBoard)[objectNewRow][objectNewCol][0]->getType() == M)
        {
            // if a tank stepped on a mine - they both destroyed

            currMovingObjects[i]->takeAHit();
            matrixObject *explodedMine = (*gameBoard)[objectNewRow][objectNewCol][0];
            delete explodedMine;
            explodedMine = nullptr;
            (*gameBoard)[objectNewRow][objectNewCol][0] = nullptr; // remove the mine from the board

            int tanksPlayer = currMovingObjects[i]->getType() == P1T ? 1 : 2;

            if (currMovingObjects[i]->getIsAlive())
            {
                writeToFile("The tank of player number " + to_string(tanksPlayer) 
                + " stepped on a mine at (" + to_string(objectNewRow) + "," + to_string(objectNewCol) +
                                ") .\n",
                            gameMapFileName);
            }
            else
            {
                writeToFile("The mine at (" + to_string(objectNewRow) + "," + to_string(objectNewCol) +
                                ") has been explode.\n",
                            gameMapFileName);
                numOfMinesDestroyed++;
            }
        }

        else if (currMovingObjects[i]->getType() == B && (*gameBoard)[objectNewRow][objectNewCol][0]->getType() == W)
        {
            // if a bullet hit a wall - the bullet is destroyed and the wall takes a hit

            writeToFile("A bullet hit a wall at (" + to_string(objectNewRow) + "," + to_string(objectNewCol) +
                            ") .\n",
                        gameMapFileName);

            currMovingObjects[i]->takeAHit();
            matrixObject *damagedWall = (*gameBoard)[objectNewRow][objectNewCol][0];
            damagedWall->takeAHit();

            if (!damagedWall->getIsAlive())
            { // if the wall destroyed - remove from the board
                delete damagedWall;
                damagedWall = nullptr;
                (*gameBoard)[objectNewRow][objectNewCol][0] = nullptr;

                writeToFile("The wall at (" + to_string(objectNewRow) + "," + to_string(objectNewCol) +
                                ") has been destroyed.\n",
                            gameMapFileName);
                numOfWallsDestroyed++;
            }
        }

        if (!currMovingObjects[i]->getIsAlive())
        {
            // Check if the moving object has been destroyed, happened if at least one of the following occur:
            //          - The moving object collides with another moving object
            //          - The moving object is a tank, and it stepped on a mine
            //          - The moving object is a bullet, and it hit a wall

            matrixObject *destroyedObject = (*gameBoard)[objectNewRow][objectNewCol][2];
            (*gameBoard)[objectNewRow][objectNewCol][2] = nullptr; // remove the object from the board
            currMovingObjects.erase(currMovingObjects.begin() + i);    // remove the object from the moving object1 vector

            if (destroyedObject->getType() == B)
            {
                bullets.erase(
                    std::remove(bullets.begin(), bullets.end(), destroyedObject),
                    bullets.end()); // delete the destroyed bullet from the bullets in the air vector
                delete destroyedObject;
                destroyedObject = nullptr;
            }

            --i;
        }
    }
    if (!tanks[0]->getIsAlive() || !tanks[1]->getIsAlive())
    { // if at least one of the tank has been destroyed - the game over
        return true;
    }
    for (size_t i = 0; i < currMovingObjects.size(); ++i)
    { // do the actual move to all the object1 that didn't get destroyed
        if (currMovingObjects[i]->getType() == B && currMovingObjects[i]->getOldLocation()[0] != currMovingObjects[i]->getLocation()[0] 
            && currMovingObjects[i]->getOldLocation()[1] != currMovingObjects[i]->getLocation()[1]){ // If a bullet moved (after the first turn)
            writeToFile("The bullet at (" + (to_string(currMovingObjects[i]->getOldLocation()[0]) + "," + to_string(currMovingObjects[i]->getOldLocation()[1]))
                    + ") moved to (" + to_string(currMovingObjects[i]->getLocation()[0]) + "," + to_string(currMovingObjects[i]->getLocation()[1]) + ").\n", gameMapFileName);
        }
        int objectNewRow = currMovingObjects[i]->getLocation()[0];
        int objectNewCol = currMovingObjects[i]->getLocation()[1];

        (*gameBoard)[objectNewRow][objectNewCol][2] = nullptr;
        (*gameBoard)[objectNewRow][objectNewCol][1] = currMovingObjects[i];
    }
    return false;
}

bool gameManager::canMakeMove(tank &tankChoseTheMove, objMove moveChosen)
{
    if (moveChosen == moveForward)
    {
        int *newLoc = tankChoseTheMove.newLocation(numOfCols, numOfRows);
        if ((*gameBoard)[newLoc[0]][newLoc[1]][0] && (*gameBoard)[newLoc[0]][newLoc[1]][0]->getType() == W)
        {
            delete[] newLoc;
            newLoc = nullptr;
            return false;
        }
        delete[] newLoc;
        newLoc = nullptr;
        return true;
    }
    else if (moveChosen == moveBackwards)
    {
        int *newLoc = tankChoseTheMove.newLocation(numOfCols, numOfRows , true);
        if ((*gameBoard)[newLoc[0]][newLoc[1]][0] && (*gameBoard)[newLoc[0]][newLoc[1]][0]->getType() == W)
        {
            delete[] newLoc;
            newLoc = nullptr;
            return false;
        }
        delete[] newLoc;
        newLoc = nullptr;
    }
    else if (moveChosen == shoot)
    {
        if (!tankChoseTheMove.canShoot())
        {
            return false;
        }
    }
    return true;
}

void gameManager::printSummeryToLog(){
    writeToFile("\n", gameMapFileName);
    writeToFile("Game summary:\n", gameMapFileName);
    writeToFile("Turns played: " + to_string(turns) + "\n", gameMapFileName);
    writeToFile("The tank of player 1 shot " + to_string(16 - tanks[0]->getNumOfShotsLeft()) + " bullets out of 16\n", gameMapFileName);
    writeToFile("The tank of player 2 shot " + to_string(16 - tanks[1]->getNumOfShotsLeft()) + " bullets out of 16\n", gameMapFileName);
    writeToFile(to_string(numOfWallsDestroyed) + " out of " + to_string(numOfWalls) + " walls were destroyed\n", gameMapFileName);
    writeToFile(to_string(numOfMinesDestroyed) + " out of " + to_string(numOfMines) + " mines were destroyed\n", gameMapFileName);
}


void gameManager::playGame()
{
    writeToFile("Starting game\n", gameMapFileName);
    bool gameOver = false;
    currMovingObjects.push_back(tanks[0]);
    currMovingObjects.push_back(tanks[1]);

    while (!gameOver && noBulletsCnt > 0 && turns < 10000){
        turns++;
        isOddTurn = !isOddTurn;
        writeToFile("\n", gameMapFileName);
        writeToFile("Turn number " + to_string(turns) + ":\n", gameMapFileName);
        if (!(tanks[0]->hasBullets()) && !(tanks[1]->hasBullets())){
            noBulletsCnt--;
        }
        moveBullets(); // only calculate the bullets new location
        if (isOddTurn){
            makeTankMoves();
        }
        gameManager::checkCollisions();
        gameOver = makeAllMoves();
    }
    printSummeryToLog();
    if (tanks[0]->getIsAlive() && tanks[1]->getIsAlive() && (noBulletsCnt == 0))// if the game over because no bullets left
    {
        writeToFile("Game result: A tie due to lack of bullets\n", gameMapFileName);
    }
    else if (turns == 10000)
    {
        writeToFile("Game result: A tie due to max turns reached\n", gameMapFileName);
    }
    else if (!tanks[0]->getIsAlive() && !tanks[1]->getIsAlive())
    {
        writeToFile("Game result: A tie because the two tanks have been destroyed at the same time\n", gameMapFileName);
    }
    else if (!(tanks[0]->getIsAlive()))// if only one of the tanks is alive
    {
        int winnerPlayerNum = tanks[1]->getType() == P1T ? 1 : 2;
        writeToFile("Game result: Player number " + to_string(winnerPlayerNum) + " won\n", gameMapFileName);
    }
    else if (!(tanks[1]->getIsAlive()))// if only one of the tanks is alive
    {
        int winnerPlayerNum = tanks[0]->getType() == P1T ? 1 : 2;
        writeToFile("Game result: Player number " + to_string(winnerPlayerNum) + " won\n", gameMapFileName);
    }
    // destroyBoardAndObjects();
}

gameManager::gameManager(const std::string &filename) :  numOfRows(0), numOfCols(0),
turns(0), noBulletsCnt(40), isOddTurn(false), numOfWalls(0), numOfMines(0), numOfWallsDestroyed(0), numOfMinesDestroyed(0), gameBoard(nullptr), tanks(array<tank*, 2>{nullptr, nullptr})
{
    if (!createMap(filename)){
        cerr << "Error: Failed to create map from file." << endl;
        exit(EXIT_FAILURE);
    }
    string fileStem = filesystem::path(filename).stem().string();
    gameMapFileName = filesystem::path(filename).replace_extension("").string();
    gameMapFileName.replace((gameMapFileName.length() - fileStem.length()), fileStem.length(), "output_" + fileStem + ".txt");

}

gameManager::~gameManager() {
    // Clean up tanks first to avoid double deletion
    for (tank* t : tanks) {
        if (t) {
            delete t;
            t = nullptr;
        }
    }
    
    currMovingObjects.clear();
    bullets.clear();
    
    // Clean up the game board
    if (gameBoard) {
        for (int i = 0; i < numOfRows; ++i) {
            for (int j = 0; j < numOfCols; ++j) {
                if ((*gameBoard)[i][j][0]) {
                    delete (*gameBoard)[i][j][0];
                    (*gameBoard)[i][j][0] = nullptr;
                }
            }
        }
        delete gameBoard;
        gameBoard = nullptr;
    }

}

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
