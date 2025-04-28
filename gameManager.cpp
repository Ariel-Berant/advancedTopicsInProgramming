#include "gameManager.h"

#define INP_ERR_FILE "input_errors.txt"
#define GAME_LOG_FILE "log_file.txt"

using namespace std;

bool gameManager::getRowsAndColsFromFile(const string &filename) {
    int cols, rows;
    string line;
    bool gotDims = true;

    ifstream file1;
    file1.open(filename);
    if (!file1.is_open())
    {
        cerr << "Error: Could not open the file '" << filename << "'." << endl;
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
            cerr << "Error: Failed to parse dimensions from line." << endl;
            gotDims = false;
        }
    }
    else
    {
        cerr << "Error: Failed to read line from file." << endl;
        gotDims = false;
    }
    file1.close();

    return gotDims;
}

// Function creates map from a given pathname.
// If some critical error occurs(criticalErr==true), function fails and returns false(i.e return !criticalErr).
bool gameManager::createMap(const string &filename)
{
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
    if(!tank1appeared && !tank2appeared){
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

bool checkIfTank(movingObject &object1){
    if(object1.getType() == P1T || object1.getType() == P2T){
        return true;
    }
    return false;
}

void printCollisionsToLog(movingObject &object1, movingObject &object2)
{
    if(checkIfTank(object1) && checkIfTank(object2) && (object1.getIsAlive() || object2.getIsAlive())){
        // if at least one of the tanks has been destroyed by a collision between them, and we didn't notified about it
        writeToFile("A tank of player number 1 collided with a tank of player number 2.\n", GAME_LOG_FILE);
    }
    if (object1.getIsAlive() && object2.getIsAlive()){// we need to notify that the two objects been destroyed
        if ((checkIfTank(object1) && object2.getType() == B) || (object1.getType() == B && checkIfTank(object2))){
            int tanks1Player = object1.getType() == P1T ? 1 : 2;
            writeToFile("A tank of player number " + to_string(tanks1Player) + " got hit by a bullet.\n", GAME_LOG_FILE);
        }
        else if (object1.getType() == B && object2.getType() == B){
            writeToFile("The bullet at (" + (to_string(object1.getOldLocation()[0]) + "," + (to_string(object1.getOldLocation()[1])) + ") collided with the bullet at (" +
                         to_string(object2.getOldLocation()[0])) + "," + (to_string(object2.getOldLocation()[1])) + ") destroying each other.\n", GAME_LOG_FILE);
        }
    }
    else if (!object1.getIsAlive() && !object2.getIsAlive()){
        //we already notified that the two objects been destroyed thus we don't need to notify again
    }
    else if ((object1.getIsAlive() && object1.getType() == B) || (object2.getIsAlive() && object2.getType() == B)){// we need to notify that an object been destroyed
            writeToFile("A bullet has been destroyed by collision.\n", GAME_LOG_FILE);
    }
}

// This function was written with the help of chat gpt
// Function to check for collisions between moving object1
bool gameManager::checkCollisions(vector<movingObject*> &objects)
{
    for (size_t i = 0; i < objects.size(); i++)
    {
        for (size_t j = i + 1; j < objects.size(); j++)
        {
            const int *a_start = objects[i]->getOldLocation();
            const int *a_end = objects[i]->getLocation();
            const int *b_start = objects[j]->getOldLocation();
            const int *b_end = objects[j]->getLocation();
            // Check if they only touch at a start point
            bool touchesAtStartOnly =
            (isSamePoint(a_start, b_end) && !isSamePoint(a_end, b_start)) ||
            (isSamePoint(a_end, b_start) && !isSamePoint(a_start, b_end));

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
                    printCollisionsToLog(*objects[i], *objects[j]);
                    objects[i]->takeAHit();
                    objects[j]->takeAHit();
                }
            }
        }
    }

    return true;
}

void gameManager::makeTankMoves(array<tank*, 2> &tanksArr)
{
    orientation ornt;
    objMove tanksMove;
    int *newLocation = nullptr;
//    right now only two tanksArr are supported, but in future will be i < tanksArr.size(), and tanksArr[i].play() will be
//    tanksArr[i].play(gameBoard, {tanksArr without i-th member}, numOfCols, numOfRows);
    for (int i = 0; i < 2; i++)
    {
        tanksArr[i]->updateTurn();
        tanksMove = tanksArr[i]->play(*gameBoard, tanksArr[1 - i]->getLocation(), numOfCols, numOfRows);
        int tanksPlayer = tanksArr[i]->getType() == P1T ? 1 : 2;

        if (tanksArr[i]->getInBack() > 0 && tanksArr[i]->getInBack() < 3)
        {
            if(tanksMove == moveForward) {
                tanksArr[i]->setInBackwards(0);
                writeToFile("The tank of player " + to_string(tanksPlayer) + " at " + (to_string(tanksArr[i]->getLocation()[0]) + "," +
                                              to_string(tanksArr[i]->getLocation()[1]))
                            + " stayed in place.\n", GAME_LOG_FILE);
            }
            else {
                if (tanksMove != noAction) {
                    writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanksArr[i]->getLocation()[0]) + "," +
                                                  to_string(tanksArr[i]->getLocation()[1]))
                                + ") tried to make a move while in backwards mode.\n", GAME_LOG_FILE);
                }
                else {
                    tanksArr[i]->setInBackwards(tanksArr[i]->getInBack() + 1);
                    if(tanksArr[i]->getInBack() == 3){
                        newLocation = tanksArr[i]->newLocation(numOfCols, numOfRows , true);
                        tanksArr[i]->setNewLocation(newLocation[0], newLocation[1]);
                        writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + to_string(tanksArr[i]->getOldLocation()[0]) + "," + to_string(tanksArr[i]->getOldLocation()[1])
                                    + ") moved backwards to (" + to_string(tanksArr[i]->getLocation()[0]) + "," + to_string(tanksArr[i]->getLocation()[1]) + ").\n", GAME_LOG_FILE);
                    }
                    else{
                        writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanksArr[i]->getLocation()[0]) + "," +
                                                      to_string(tanksArr[i]->getLocation()[1]))
                                    + ") stayed in place waiting until he can move backwards.\n", GAME_LOG_FILE);
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
                ornt = tanksArr[i]->getOrientation();
                tanksArr[i]->setOrientation(orientation((8 + tanksArr[i]->getOrientation() - 5 + tanksMove) % 8));
                writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanksArr[i]->getLocation()[0]) + "," + to_string(tanksArr[i]->getLocation()[1]))
                            + ") turned " + to_string(45 * abs(ornt - tanksArr[i]->getOrientation())) + " degrees"
                            + (tanksMove<5 ? " clockwise.\n":" counter-clockwise.\n"), GAME_LOG_FILE);
                tanksArr[i]->setInBackwards(0);
                break;
            case noAction:
                writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanksArr[i]->getLocation()[0]) + "," + to_string(tanksArr[i]->getLocation()[1]))
                + ") stayed in place.\n", GAME_LOG_FILE);
                tanksArr[i]->setInBackwards(0);
                break;
            case moveForward:
                tanksArr[i]->setInBackwards(0);
                if (canMakeMove(*tanksArr[i], tanksMove))
                {
                    newLocation = tanksArr[i]->newLocation(numOfCols, numOfRows);
                    tanksArr[i]->setNewLocation(newLocation[0], newLocation[1]);
                    writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanksArr[i]->getOldLocation()[0]) + "," + to_string(tanksArr[i]->getOldLocation()[1]))
                    + ") went forwards to (" + to_string(tanksArr[i]->getLocation()[0]) + "," + to_string(tanksArr[i]->getLocation()[1]) + ").\n", GAME_LOG_FILE);
                }
                else
                {
                    writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanksArr[i]->getLocation()[0]) + "," + to_string(tanksArr[i]->getLocation()[1]))
                    + ") tried to move forwards when he couldn't (unstoppable force met an immovable object).\n", GAME_LOG_FILE);
                }
                break;
            case moveBackwards:
                if (canMakeMove(*tanksArr[i], tanksMove))
                {
                    if (tanksArr[i]->getInBack() >= 3) // if we have moved backwards last turn and want to move
                    {
                        tanksArr[i]->setInBackwards(tanksArr[i]->getInBack() + 1);
                        newLocation = tanksArr[i]->newLocation(numOfCols, numOfRows , true);
                        tanksArr[i]->setNewLocation(newLocation[0], newLocation[1]);
                        writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanksArr[i]->getLocation()[0]) + "," + to_string(tanksArr[i]->getLocation()[1]))
                        + ") moved backwards to (" + to_string(tanksArr[i]->getLocation()[0]) + "," + to_string(tanksArr[i]->getLocation()[1]) + ").\n", GAME_LOG_FILE);
                    }
                    else
                    {
                        tanksArr[i]->setInBackwards(1);
                        writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanksArr[i]->getLocation()[0]) + "," + to_string(tanksArr[i]->getLocation()[1]))
                        + ") stayed in place waiting until he can move backwards.\n", GAME_LOG_FILE);
                    }
                }
                else
                {
                    writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanksArr[i]->getLocation()[0]) + "," + to_string(tanksArr[i]->getLocation()[1]))
                    + ") tried to move backwards when he couldn't (unstoppable force met an immovable object).\n", GAME_LOG_FILE);
                    tanksArr[i]->setInBackwards(0);
                }
                break;
            case shoot:
                if (canMakeMove(*tanksArr[i], tanksMove))
                {
                    newLocation = tanksArr[i]->newLocation(numOfCols, numOfRows);               // Get bullet location
                    bullet *b = new bullet(newLocation[0], newLocation[1], tanksArr[i]->getOrientation(), B); // Create bullet
                    currMovingObjects.push_back(b);
                    bullets.push_back(b);
                    tanksArr[i]->useShot();
                    delete[] newLocation;
                    newLocation = nullptr; // Set to nullptr to enter the else statement(at 430)
                    writeToFile("The tank of player " + to_string(tanksPlayer) + " at (" + (to_string(tanksArr[i]->getLocation()[0]) + "," + to_string(tanksArr[i]->getLocation()[1]))
                        + ") shot a bullet.\n", GAME_LOG_FILE);
                    writeToFile("The new bullet location is (" + (to_string(b->getLocation()[0]) + "," + to_string(b->getLocation()[1])) + ").\n", GAME_LOG_FILE);
                }
                else
                {
                    writeToFile("The tank of player number " + to_string(tanksArr[i]->getType() == P1T ? 1 : 2) + " shot a blank.\n", GAME_LOG_FILE);
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
            tanksArr[i]->setNewLocation(tanksArr[i]->getLocation()[0], tanksArr[i]->getLocation()[1]);
        }
    }
}


bool gameManager::makeAllMoves(vector<movingObject*> &movingObjects)//return true if the game is over and false otherwise
{
    for (size_t i = 0; i < movingObjects.size(); ++i)
    {

        const int objectNewRow = movingObjects[i]->getLocation()[0];
        const int objectNewCol = movingObjects[i]->getLocation()[1];

        (*gameBoard)[movingObjects[i]->getOldLocation()[0]][movingObjects[i]->getOldLocation()[1]][1] = nullptr;
        (*gameBoard)[objectNewRow][objectNewCol][2] = movingObjects[i];

        if (!(*gameBoard)[objectNewRow][objectNewCol][0]){}
        else if ((movingObjects[i]->getType() == P1T || movingObjects[i]->getType() == P2T) && (*gameBoard)[objectNewRow][objectNewCol][0]->getType() == M)
        {
            // if a tank stepped on a mine - they both destroyed

            movingObjects[i]->takeAHit();
            matrixObject *explodedMine = (*gameBoard)[objectNewRow][objectNewCol][0];
            delete explodedMine;
            (*gameBoard)[objectNewRow][objectNewCol][0] = nullptr; // remove the mine from the board

            int tanksPlayer = movingObjects[i]->getType() == P1T ? 1 : 2;

            if (movingObjects[i]->getIsAlive())
            {
                writeToFile("The tank of player number " + to_string(tanksPlayer) 
                + " stepped on a mine at (" + to_string(objectNewRow) + "," + to_string(objectNewCol) +
                                ") .\n",
                            GAME_LOG_FILE);
            }
            else
            {
                writeToFile("The mine at (" + to_string(objectNewRow) + "," + to_string(objectNewCol) +
                                ") has been explode.\n",
                            GAME_LOG_FILE);
            }
        }

        else if (movingObjects[i]->getType() == B && (*gameBoard)[objectNewRow][objectNewCol][0]->getType() == W)
        {
            // if a bullet hit a wall - the bullet is destroyed and the wall takes a hit

            writeToFile("A bullet hit a wall at (" + to_string(objectNewRow) + "," + to_string(objectNewCol) +
                            ") .\n",
                        GAME_LOG_FILE);

            movingObjects[i]->takeAHit();
            matrixObject *damagedWall = (*gameBoard)[objectNewRow][objectNewCol][0];
            damagedWall->takeAHit();

            if (!damagedWall->getIsAlive())
            { // if the wall destroyed - remove from the board
                delete damagedWall;
                (*gameBoard)[objectNewRow][objectNewCol][0] = nullptr;

                writeToFile("The wall at (" + to_string(objectNewRow) + "," + to_string(objectNewCol) +
                                ") has been destroyed.\n",
                            GAME_LOG_FILE);
            }
        }

        if (!movingObjects[i]->getIsAlive())
        {
            // Check if the moving object has been destroyed, happened if at least one of the following occur:
            //          - The moving object collides with another moving object
            //          - The moving object is a tank, and it stepped on a mine
            //          - The moving object is a bullet, and it hit a wall

            matrixObject *destroyedObject = (*gameBoard)[objectNewRow][objectNewCol][2];
            (*gameBoard)[objectNewRow][objectNewCol][2] = nullptr; // remove the object from the board
            movingObjects.erase(movingObjects.begin() + i);    // remove the object from the moving object1 vector

            if (destroyedObject->getType() == B)
            {
                bullets.erase(
                    std::remove(bullets.begin(), bullets.end(), destroyedObject),
                    bullets.end()); // delete the destroyed bullet from the bullets in the air vector
                delete destroyedObject;
            }

            --i;
        }
    }
    if (!tanks[0]->getIsAlive() || !tanks[1]->getIsAlive())
    { // if at least one of the tank has been destroyed - the game over
        return true;
    }
    for (size_t i = 0; i < movingObjects.size(); ++i)
    { // do the actual move to all the object1 that didn't get destroyed
        if (movingObjects[i]->getType() == B && movingObjects[i]->getOldLocation()[0] != movingObjects[i]->getLocation()[0] 
            && movingObjects[i]->getOldLocation()[1] != movingObjects[i]->getLocation()[1]){ // If a bullet moved (after the first turn)
            writeToFile("The bullet at (" + (to_string(movingObjects[i]->getOldLocation()[0]) + "," + to_string(movingObjects[i]->getOldLocation()[1]))
                    + ") moved to (" + to_string(movingObjects[i]->getLocation()[0]) + "," + to_string(movingObjects[i]->getLocation()[1]) + ").\n", GAME_LOG_FILE);
        }
        int objectNewRow = movingObjects[i]->getLocation()[0];
        int objectNewCol = movingObjects[i]->getLocation()[1];

        (*gameBoard)[objectNewRow][objectNewCol][2] = nullptr;
        (*gameBoard)[objectNewRow][objectNewCol][1] = movingObjects[i];
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
            return false;
        }
        delete[] newLoc;
        return true;
    }
    else if (moveChosen == moveBackwards)
    {
        int *newLoc = tankChoseTheMove.newLocation(numOfCols, numOfRows , true);
        if ((*gameBoard)[newLoc[0]][newLoc[1]][0] && (*gameBoard)[newLoc[0]][newLoc[1]][0]->getType() == W)
        {
            delete[] newLoc;
            return false;
        }
        delete[] newLoc;
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
    writeToFile("\n", GAME_LOG_FILE);
    writeToFile("Game summary:\n", GAME_LOG_FILE);
    writeToFile("Turns played: " + to_string(turns) + "\n", GAME_LOG_FILE);
    writeToFile("The tank of player 1 shot: " + to_string(16 - tanks[0]->getNumOfShotsLeft()) + " out of 16\n", GAME_LOG_FILE);
    writeToFile("The tank of player 2 shot: " + to_string(16 - tanks[1]->getNumOfShotsLeft()) + " out of 16\n", GAME_LOG_FILE);
}


void gameManager::playGame()
{
    writeToFile("Starting game\n", GAME_LOG_FILE);
    bool gameOver = false;
    currMovingObjects.push_back(tanks[0]);
    currMovingObjects.push_back(tanks[1]);

    while (!gameOver && noBulletsCnt > 0 && turns < 10000){
        turns++;
        isOddTurn = !isOddTurn;
        writeToFile("\n", GAME_LOG_FILE);
        writeToFile("Turn number " + to_string(turns) + ":\n", GAME_LOG_FILE);
        if (!(tanks[0]->hasBullets()) && !(tanks[1]->hasBullets())){
            noBulletsCnt--;
        }
        moveBullets(); // only calculate the bullets new location
        if (isOddTurn){
            makeTankMoves(tanks);
        }
        gameManager::checkCollisions(currMovingObjects);
        gameOver = makeAllMoves(currMovingObjects);
    }
    printSummeryToLog();
    if (tanks[0]->getIsAlive() && tanks[1]->getIsAlive() && (noBulletsCnt == 0))// if the game over because no bullets left
    {
        writeToFile("Game result: A tie due to lack of bullets\n", GAME_LOG_FILE);
    }
    else if (turns == 10000)
    {
        writeToFile("Game result: A tie due to max turns reached\n", GAME_LOG_FILE);
    }
    else if (!tanks[0]->getIsAlive() && !tanks[1]->getIsAlive())
    {
        writeToFile("Game result: A tie because the two tanks have been destroyed at the same time\n", GAME_LOG_FILE);
    }
    else if (!(tanks[0]->getIsAlive()))// if only one of the tanks is alive
    {
        int winnerPlayerNum = tanks[1]->getType() == P1T ? 1 : 2;
        writeToFile("Game result: Player number " + to_string(winnerPlayerNum) + " won\n", GAME_LOG_FILE);
    }
    else if (!(tanks[1]->getIsAlive()))// if only one of the tanks is alive
    {
        int winnerPlayerNum = tanks[0]->getType() == P1T ? 1 : 2;
        writeToFile("Game result: Player number " + to_string(winnerPlayerNum) + " won\n", GAME_LOG_FILE);
    }
    // destroyBoardAndObjects();
}

gameManager::gameManager(const std::string &filename) :  numOfRows(0), numOfCols(0),
turns(0), noBulletsCnt(40), isOddTurn(false), gameBoard(nullptr), tanks(array<tank*, 2>{nullptr, nullptr})
{
    if (!createMap(filename)){
        cerr << "Error: Failed to create map from file." << endl;
        exit(EXIT_FAILURE);
    }
}

gameManager::~gameManager(){
    if (!gameBoard){
        // for (int i = 0; i < numOfRows; ++i){
        //     for (int j = 0; j < numOfCols; ++j){
        //         for (int k = 0; k < 2; ++k){
        //             if (!(*gameBoard)[i][j][k]){
        //                 delete (*gameBoard)[i][j][k];
        //                 (*gameBoard)[i][j][k] = nullptr;
        //             }
        //         }
        //     }
        // }
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
