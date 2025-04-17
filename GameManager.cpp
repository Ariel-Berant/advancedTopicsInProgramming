#include <algorithm>

#include "gameManager.h"

#define INP_ERR_FILE "input_errors.txt"
#define GAME_LOG_FILE "log_file.txt"
#define isCritErr(bool)   \
    {                     \
        if (!bool)        \
        {                 \
            return false; \
        }                 \
    }

using namespace std;

// Function creates map from a given pathname.
// If some critical error occurs(criticalErr==true), function fails and returns false(i.e return !criticalErr).
bool gameManager::createMap(const string &filename)
{
    ifstream file1;
    file1.open(filename);
    if (!file1.is_open())
    {
        cerr << "Error: Could not open the file '" << filename << "'." << endl;
        return false;
    }
    int rows, cols, currRow = 0, currCol = 0;
    string line;

    if (getline(file1, line))
    {
        istringstream iss(line);
        if (iss >> cols >> rows)
        {
            numOfCols = cols;
            numOfRows = rows;
        }
        else
        {
            cerr << "Error: Failed to parse dimensions from line." << endl;
            return false;
        }
    }
    else
    {
        cerr << "Error: Failed to read line from file." << endl;
        return false;
    }

    gameBoard = new vector<vector<array<matrixObject *, 3>>>(rows, vector<array<matrixObject *, 3>>(cols));
    tanks[0] = nullptr;
    tanks[1] = nullptr;

    while (getline(file1, line))
    {
        if (currRow == numOfRows)
        {
            isCritErr(writeToFile("Error: Too many rows in the map file.\n", INP_ERR_FILE));
            break;
        }
        for (char ch : line)
        {
            if (currCol == numOfCols)
            {
                isCritErr(writeToFile("Error: Too many columns in row " + to_string(currRow) + ".\n", INP_ERR_FILE));
                break;
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
                    tanks[0] = new tank(currRow, currCol, L, P1T);
                    (*gameBoard)[currRow][currCol][1] = tanks[0];
                }
                else
                {
                    isCritErr(writeToFile("Error: More than one tank for player 1.\n", INP_ERR_FILE));
                    (*gameBoard)[currRow][currCol][1] = nullptr;
                }
                (*gameBoard)[currRow][currCol][0] = nullptr;
                (*gameBoard)[currRow][currCol][2] = nullptr;
                break;
            case '2':
                if (!tanks[1])
                {
                    tanks[1] = new tank(currRow, currCol, R, P2T);
                    (*gameBoard)[currRow][currCol][1] = tanks[1];
                }
                else
                {
                    isCritErr(writeToFile("Error: More than one tank for player 2.\n", INP_ERR_FILE));
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
                isCritErr(writeToFile("Error: unrecognized character, ASCII #'" + to_string(ch) + "' in the map file.\n", INP_ERR_FILE));
                (*gameBoard)[currRow][currCol][0] = nullptr;
                (*gameBoard)[currRow][currCol][1] = nullptr;
                (*gameBoard)[currRow][currCol][2] = nullptr;
            }
            currCol++;
        }
        if (currCol < numOfCols)
        {
            isCritErr(writeToFile("Error: Not enough columns in row " + to_string(currRow) + ".\n", INP_ERR_FILE));
        }
        while (currCol < numOfCols)
        {
            (*gameBoard)[currRow][currCol][0] = nullptr;
        }
        currRow++;
        currCol = 0;
    }
    if (currRow < numOfRows)
    {
        isCritErr(writeToFile("Error: Not enough rows in the map file.\n", INP_ERR_FILE));
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
    for (bullet b : bullets)
    {
        newLoc = b.newLocation(numOfCols, numOfRows);
        b.setNewLocation(newLoc[0], newLoc[1]);
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

bool printCollisionsToLog(movingObject &object1, movingObject &object2)
{
    if (object1.getIsAlive() && object2.getIsAlive())
    {
        if ((object1.getType() == P1T && object2.getType() == B) || (object1.getType() == B && object2.getType() == P1T))
        {
            return writeToFile("A tank of player number 1 got hit by a bullet.\n", GAME_LOG_FILE);
        }
        else if ((object1.getType() == P2T && object2.getType() == B) || (object1.getType() == B && object2.getType() == P2T))
        {
            return writeToFile("A tank of player number 2 got hit by a bullet.\n", GAME_LOG_FILE);
        }
        else if ((object1.getType() == P1T && object2.getType() == P2T) || (object1.getType() == P2T && object2.getType() == P1T))
        {
            return writeToFile("A tank of player number 1 collided with a tank of player number 2.\n", GAME_LOG_FILE);
        }
        else if (object1.getType() == B && object2.getType() == B)
        {
            return writeToFile("Two bullets have collided, destroying each other.\n", GAME_LOG_FILE);
        }
    }
    else if (!object1.getIsAlive() && !object2.getIsAlive())
    {
    }
    else if (object2.getIsAlive())
    {
        if (object2.getType() == B)
        {
            return writeToFile("A bullet has been destroyed by collision.\n", GAME_LOG_FILE);
        }
        else if ((object1.getType() == P2T && object2.getType() == P1T) || (object1.getType() == P1T && object2.getType() == P2T))
        {
            return writeToFile("A tank of player number 1 collided with a tank of player number 2.\n",
                               GAME_LOG_FILE);
        }
    }
    else
    {
        if (object1.getType() == B)
        {
            return writeToFile("A bullet has been destroyed by collision.\n", GAME_LOG_FILE);
        }
        else if ((object1.getType() == P2T && object2.getType() == P1T) || (object1.getType() == P1T && object2.getType() == P2T))
        {
            return writeToFile("A tank of player number 1 collided with a tank of player number 2.\n",
                               GAME_LOG_FILE);
        }
    }
}

// This function was written with the help of chat gpt
// Function to check for collisions between moving object1
bool gameManager::checkCollisions(vector<movingObject> &objects)
{
    for (size_t i = 0; i < objects.size(); ++i)
    {
        for (size_t j = i + 1; j < objects.size(); ++j)
        {
            const int *a_start = objects[i].getOldLocation();
            const int *a_end = objects[i].getLocation();
            const int *b_start = objects[j].getOldLocation();
            const int *b_end = objects[j].getLocation();

            if (doLinesIntersect(a_start, a_end, b_start, b_end))
            {
                // Check if they only touch at a start point
                bool touchesAtStartOnly =
                    isSamePoint(a_start, b_end) ||
                    isSamePoint(a_end, b_start);

                // Check if the segments are collinear
                bool collinear =
                    areCollinear(a_start, a_end, b_start) &&
                    areCollinear(a_start, a_end, b_end);

                bool endAtSamePoint =
                    isSamePoint(a_end, b_end);

                // If they don't touch only at the start points or are collinear, they collide
                // and we need to check if they don't finish at the same point
                if ((!touchesAtStartOnly || collinear) || endAtSamePoint)
                {
                    isCritErr(printCollisionsToLog(objects[i], objects[j]));
                    objects[i].takeAHit();
                    objects[j].takeAHit();
                }
            }
        }
    }

    return true;
}

// TODO: Add logging, specifically for "bad steps"(anything other than a forward move in backwards mode)
bool gameManager::makeTankMoves(vector<tank> &tanks)
{
    orientation ornt;
    objMove tanksMove;
    int *newLocation = nullptr;
    for (int i = 0; i < tanks.size(); i++)
    {
        tanks[i].updateTurnsUntilNextShot();
        // tanksmove = tanks[i].play();
        if (tanks[i].getInBack() > 1)
        {
            if (tanksMove == moveForward)
            {
                tanks[i].setInBackwards(0);
                writeToFile("The tank at " + (to_string(tanks[i].getOldLocation()[0]) + "," + to_string(tanks[i].getOldLocation()[1]))
                + " stayed in place.\n", GAME_LOG_FILE);
            }
            else
            {
                if (tanks[i].getInBack() == 2) // if we got a request to move backwards two turns ago
                {
                    newLocation = tanks[i].newLocationAtReverse(numOfCols, numOfRows);
                    tanks[i].setNewLocation(newLocation[0], newLocation[1]);
                }
                else
                {// To avoid the else statement(at 430) from being executed when the tank is in backwards mode
                    newLocation = new int[2];
                    newLocation[0] = 0;
                }
                if (tanksMove != noAction) // if we got a request to move forward or rotate
                {
                    writeToFile("The tank at " + (to_string(tanks[i].getOldLocation()[0]) + "," + to_string(tanks[i].getOldLocation()[1]))
                    + " tried to move while in backwards mode, so it stayed in place.\n", GAME_LOG_FILE);
                }
                else{
                    writeToFile("The tank at " + (to_string(tanks[i].getOldLocation()[0]) + "," + to_string(tanks[i].getOldLocation()[1]))
                    + " stayed in place.\n", GAME_LOG_FILE);
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
                ornt = tanks[i].getOrientation();
                tanks[i].setOrientation(orientation((8 + tanks[i].getOrientation() - 5 + tanksMove) % 8));
                writeToFile("The tank at " + (to_string(tanks[i].getOldLocation()[0]) + "," + to_string(tanks[i].getOldLocation()[1]))
                + " turned" + to_string(45 * abs(ornt - tanks[i].getOrientation())) + " degrees" 
                + (tanksMove<5 ? " clockwise.\n":" counter-clockwise.\n"), GAME_LOG_FILE);
                break;
            case noAction:
                writeToFile("The tank at " + (to_string(tanks[i].getOldLocation()[0]) + "," + to_string(tanks[i].getOldLocation()[1]))
                + " stayed in place.\n", GAME_LOG_FILE);
                break;
            case moveForward:
                if (canMakeMove(tanks[i], tanksMove))
                {
                    newLocation = tanks[i].newLocation(numOfCols, numOfRows);
                    tanks[i].setNewLocation(newLocation[0], newLocation[1]);
                    writeToFile("The tank at (" + (to_string(tanks[i].getOldLocation()[0]) + "," + to_string(tanks[i].getOldLocation()[1]))
                    + ") went forwards.\n", GAME_LOG_FILE);
                }
                else
                {
                    writeToFile("The tank at (" + (to_string(tanks[i].getOldLocation()[0]) + "," + to_string(tanks[i].getOldLocation()[1]))
                    + ") tried to move forwards when he couldn't(unstoppable force met an immovable object).\n", GAME_LOG_FILE);
                }
                break;
            case moveBackwards:
                if (canMakeMove(tanks[i], tanksMove))
                {
                    if (tanks[i].getInBack() == 1) // if we have moved backwards last turn and want to move
                    {
                        tanks[i].setInBackwards(2);
                        newLocation = tanks[i].newLocationAtReverse(numOfCols, numOfRows);
                        tanks[i].setNewLocation(newLocation[0], newLocation[1]);
                        writeToFile("The tank at  " + to_string(tanks[i].getType() == P1T ? 1 : 2)
                        + " moved backwards.\n", GAME_LOG_FILE);
                    }
                    else
                    {
                        tanks[i].setInBackwards(3);
                        writeToFile("The tank at " + (to_string(tanks[i].getOldLocation()[0]) + "," + to_string(tanks[i].getOldLocation()[1]))
                        + " stayed in place.\n", GAME_LOG_FILE);
                    }
                }
                else
                {
                    writeToFile("A tank of player number " + to_string(tanks[i].getType() == P1T ? 1 : 2) 
                    + " tried to move backwards when he couldn't(unstoppable force met an immovable object).\n", GAME_LOG_FILE);
                }
                break;
            case shoot:
                if (canMakeMove(tanks[i], tanksMove)) // TODO: check if can happen when other tank is where the bullet will spawn
                {
                    newLocation = tanks[i].newLocation(numOfCols, numOfRows);               // Get bullet location
                    bullet b(newLocation[0], newLocation[1], tanks[i].getOrientation(), B); // Create bullet
                    b.setNewLocation(newLocation[0], newLocation[1]);                       // Set both bullet locations to be the same
                    (*gameBoard)[newLocation[0]][newLocation[1]][2] = &b;                   // Add bullet to the game board
                    bullets.push_back(b);
                    tanks[i].useShot();
                    newLocation = nullptr; // Set to nullptr to enter the else statement(at 430)
                }
                else
                {
                    writeToFile("A tank of player number " + to_string(tanks[i].getType() == P1T ? 1 : 2) + " shot a blank(no ammo left).\n", GAME_LOG_FILE);
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
            tanks[i].setNewLocation(tanks[i].getOldLocation()[0], tanks[i].getOldLocation()[1]);
        }
        

        // Check if the tank is in backwards mode and update its status
        // Wrote here since I had already finished the function, and moving to top of the function would give me a headache
        if (tanks[i].getInBack() > 0)
        {
            tanks[i].setInBackwards(tanks[i].getInBack() - 1);
        }
        
    }
} // get the tanks array
// for every tank tank.play()
// check if the move is valid
// if the move is valid - make the move

bool gameManager::makeAllMoves(vector<movingObject> &movingObjects)
{
    for (size_t i = 0; i < movingObjects.size(); ++i)
    {

        const int objectNewX = movingObjects[i].getLocation()[0];
        const int objectNewY = movingObjects[i].getLocation()[1];

        (*gameBoard)[movingObjects[i].getOldLocation()[0]][movingObjects[i].getOldLocation()[1]][1] = nullptr;
        (*gameBoard)[objectNewX][objectNewY][2] = &movingObjects[i];

        if ((movingObjects[i].getType() == P1T || movingObjects[i].getType() == P2T) && (*gameBoard)[objectNewX][objectNewY][0]->getType() == M)
        {
            // if a tank stepped on a mine - they both destroyed

            movingObjects[i].takeAHit();
            matrixObject *explodedMine = (*gameBoard)[objectNewX][objectNewY][0];
            delete explodedMine;
            (*gameBoard)[objectNewX][objectNewY][0] = nullptr; // remove the mine from the board

            int tanksPlayer = movingObjects[i].getType() == P1T ? 1 : 2;

            if (movingObjects[i].getIsAlive())
            {
                writeToFile("A tank of player number " + to_string(tanksPlayer) 
                + " stepped on a mine at (" + to_string(objectNewX) + "," + to_string(objectNewY) +
                                ") .\n",
                            GAME_LOG_FILE);
            }
            else
            {
                writeToFile("The mine at (" + to_string(objectNewX) + "," + to_string(objectNewY) +
                                ") has been expload.\n",
                            GAME_LOG_FILE);
            }
        }

        else if (movingObjects[i].getType() == B && (*gameBoard)[objectNewX][objectNewY][0]->getType() == W)
        {
            // if a bullet hit a wall - the bullet is destroyed and the wall takes a hit

            writeToFile("A bullet hit a wall at (" + to_string(objectNewX) + "," + to_string(objectNewY) +
                            ") .\n",
                        GAME_LOG_FILE);

            if (movingObjects[i].getIsAlive())
            {
                writeToFile("A bullet hit a wall at (" + to_string(objectNewX) + "," + to_string(objectNewY) +
                                ") .\n",
                            GAME_LOG_FILE);
            }
            else
            {
                writeToFile("The wall at (" + to_string(objectNewX) + "," + to_string(objectNewY) +
                                ") has been hit by a bullet.\n",
                            GAME_LOG_FILE);
            }

            movingObjects[i].takeAHit();
            matrixObject *damagedWall = (*gameBoard)[objectNewX][objectNewY][0];
            damagedWall->takeAHit();

            if (!damagedWall->getIsAlive())
            { // if the wall destroyed - remove from the board
                delete damagedWall;
                (*gameBoard)[objectNewX][objectNewY][0] = nullptr;

                writeToFile("The wall at (" + to_string(objectNewX) + "," + to_string(objectNewY) +
                                ") has been destroyed.\n",
                            GAME_LOG_FILE);
            }
        }

        if (!movingObjects[i].getIsAlive())
        {
            // Check if the moving object has been destroyed, happened if at least one of the following occur:
            //          - The moving object collides with another moving object
            //          - The moving object is a tank, and it stepped on a mine
            //          - The moving object is a bullet, and it hit a wall

            matrixObject *destroyedObject = (*gameBoard)[objectNewX][objectNewY][2];
            (*gameBoard)[objectNewX][objectNewY][2] = nullptr; // remove the object from the board
            movingObjects.erase(movingObjects.begin() + i);    // remove the object from the moving object1 vector

            if (destroyedObject->getType() == B)
            {
                bullets.erase(
                    std::remove_if(bullets.begin(), bullets.end(),
                                   [&](const bullet &item)
                                   {
                                       return &item == &movingObjects[i]; // Compare references
                                   }),
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
        int objectNewX = movingObjects[i].getLocation()[0];
        int objectNewY = movingObjects[i].getLocation()[1];

        (*gameBoard)[objectNewX][objectNewY][2] = nullptr;
        (*gameBoard)[objectNewX][objectNewY][1] = &movingObjects[i];
    }
    return false;
}

bool gameManager::canMakeMove(tank &tankChoseTheMove, objMove moveChosen)
{
    if (moveChosen == moveForward)
    {
        int *newLoc = tankChoseTheMove.newLocation(numOfCols, numOfRows);
        if ((*gameBoard)[newLoc[0]][newLoc[1]][0]->getType() == W)
        {
            return false;
        }
        return true;
    }
    else if (moveChosen == moveBackwards)
    {
        int *newLoc = tankChoseTheMove.newLocationAtReverse(numOfCols, numOfRows);
        if ((*gameBoard)[newLoc[0]][newLoc[1]][0]->getType() == W)
        {
            return false;
        }
        return true;
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

void gameManager::playGame()
{
    writeToFile("Starting game\n", GAME_LOG_FILE);
    bool gameOver = false;
    objMove p1Move, p2Move;
    currMovingObjects.push_back(*tanks[0]);
    currMovingObjects.push_back(*tanks[1]);

    while (!gameOver && noBulletsCnt > 0)
    {
        if (!(tanks[0]->canShoot()) && !(tanks[1]->canShoot()))
        {
            noBulletsCnt--;
        }
        moveBullets(); // only calculate the bullets new location
        if (isOddTurn)
        {
            //            p1Move = tanks[0].play();
            //            p2Move = tanks[1].play();
        }
        gameManager::checkCollisions(currMovingObjects);
        gameOver = makeAllMoves(currMovingObjects);
    }
    if (tanks[0]->getIsAlive() && tanks[1]->getIsAlive() && (noBulletsCnt == 0))
    {
        writeToFile("Game result: A tie\n", GAME_LOG_FILE);
    }

    else if (!(tanks[0]->getIsAlive()) && !(tanks[1]->getIsAlive()))
    {
        writeToFile("Game result: A tie\n", GAME_LOG_FILE);
    }
    else if (!(tanks[0]->getIsAlive()))
    {
        int winnerPlayerNum = tanks[0]->getType() == P1T ? 2 : 1;
        writeToFile("Game result: Player number " + to_string(winnerPlayerNum) + " won\n", GAME_LOG_FILE);
    }
    // destroyBoardAndObjects();
}

//        TODO: Make a log file.
//        TODO: Implement above functions

gameManager::gameManager(const std::string &filename)
{
    numOfCols = 0;
    numOfRows = 0;
    createMap(filename);
    noBulletsCnt = 40;
}

gameManager::~gameManager()
{
    for (int i = 0; i < numOfRows; ++i)
    {
        for (int j = 0; j < numOfCols; ++j)
        {
            for (int k = 0; k < 2; ++k)
            {
                if (!(*gameBoard)[i][j][k])
                {
                    delete (*gameBoard)[i][j][k];
                    (*gameBoard)[i][j][k] = nullptr;
                }
            }
        }
    }
    delete gameBoard;
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