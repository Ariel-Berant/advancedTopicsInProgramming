#include "gameManager.h"

#define INP_ERR_FILE "input_errors.txt"
#define GAME_LOG_FILE "log_file.txt"
#define isCritErr(bool) { \
if(!bool){return false;}                 \
\
}

using namespace std;

//wtf is wrong here


// Function creates map from a given pathname.
// If some critical error occurs(criticalErr==true), function fails and returns false(i.e return !criticalErr).
bool gameManager::createMap(const string &filename) {
    ifstream file1;

    file1.open(filename);
    if (!file1.is_open()) {
        cerr << "Error: Could not open the file '" << filename << "'." << endl;
        return false;
    }
    int rows, cols, currRow = 0, currCol = 0;
    string line;

    if (getline(file1, line)) {
        istringstream iss(line);
        if (iss >> cols >> rows) {
            numOfCols = cols;
            numOfRows = rows;
        } else {
            cerr << "Error: Failed to parse dimensions from line." << endl;
            return false;
        }
    } else {
        cerr << "Error: Failed to read line from file." << endl;
        return false;

    }

    gameBoard = new vector<vector<array<matrixObject *, 3>>>(rows, vector<array<matrixObject *, 3>>(cols));
    tanks[0] = nullptr;
    tanks[1] = nullptr;

    while (getline(file1, line)) {
        if (currRow == numOfRows) {
            isCritErr(writeToFile("Error: Too many rows in the map file.\n", INP_ERR_FILE));
            break;
        }
        for (char ch: line) {
            if (currCol == numOfCols) {
                isCritErr(writeToFile("Error: Too many columns in row " + to_string(currRow) + ".\n", INP_ERR_FILE));
                break;
            }
            switch (ch) {
                case '#':
                    (*gameBoard)[currRow][currCol][0] = new wall(currRow, currCol);
                    break;
                case '1':
                    if (!tanks[0]) {
                        tanks[0] = new tank(currRow, currCol, L);
                        (*gameBoard)[currRow][currCol][0] = tanks[0];
                    }
                    else{
                        isCritErr(writeToFile("Error: More than one tank for player 1.\n", INP_ERR_FILE));
                        (*gameBoard)[currRow][currCol][0] = nullptr;
                    }
                    break;
                case '2':
                    if (!tanks[1]) {
                        tanks[1] = new tank(currRow, currCol, R);
                        (*gameBoard)[currRow][currCol][0] = tanks[1];
                    }
                    else{
                        isCritErr(writeToFile("Error: More than one tank for player 2.\n", INP_ERR_FILE));
                        (*gameBoard)[currRow][currCol][0] = nullptr;
                    }
                    break;
                case '@':
                    (*gameBoard)[currRow][currCol][0] = new mine(currRow, currCol);
                    break;
                case ' ':
                    (*gameBoard)[currRow][currCol][0] = nullptr;
                    break;
                default:
                    isCritErr(writeToFile("Error: unrecognized character, ASCII #'"+to_string(ch)+"' in the map file.\n", INP_ERR_FILE));
                    (*gameBoard)[currRow][currCol][0] = nullptr;
            }
            currCol++;
        }
        if (currCol < numOfCols){
            isCritErr(writeToFile("Error: Not enough columns in row " + to_string(currRow) + ".\n", INP_ERR_FILE));
        }
        while(currCol < numOfCols){
            (*gameBoard)[currRow][currCol][0] = nullptr;
        }
        currRow++;
        currCol = 0;
    }
    if (currRow < numOfRows){
        isCritErr(writeToFile("Error: Not enough rows in the map file.\n", INP_ERR_FILE));
    }
    while(currRow < numOfRows){
        for (int i = 0; i < numOfCols; ++i) {
            (*gameBoard)[currRow][i][0] = nullptr;
        }
        currRow++;
    }

    file1.close();
    return true;

}


void gameManager::moveBullets() {
    int *newLoc;
    for(bullet b : bullets) {
        newLoc = b.newLocation(numOfCols, numOfRows);
        b.setLocation(newLoc[0], newLoc[1]);
    }
}

// Helper: Check if two points are the same
bool isSamePoint(const int* a, const int* b) {
    return a[0] == b[0] && a[1] == b[1];
}

// Helper: Check if three points are collinear
bool areCollinear(const int* p, const int* q, const int* r) {
    return (q[1] - p[1]) * (r[0] - q[0]) == (q[0] - p[0]) * (r[1] - q[1]);
}


// // Helper function to check if two line segments intersect
bool doLinesIntersect(const int* p1, const int* q1, const int* p2, const int* q2) {
    auto orientation = [](const int* p, const int* q, const int* r) {
        int val = (q[1] - p[1]) * (r[0] - q[0]) - (q[0] - p[0]) * (r[1] - q[1]);
        if (val == 0) return 0; // Collinear
        return (val > 0) ? 1 : 2; // Clockwise or Counterclockwise
    };

    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4) return true;

    // Special cases (collinear points)
    auto onSegment = [](const int* p, const int* q, const int* r) {
        return q[0] <= max(p[0], r[0]) && q[0] >= min(p[0], r[0]) &&
               q[1] <= max(p[1], r[1]) && q[1] >= min(p[1], r[1]);
    };

    if (o1 == 0 && onSegment(p1, p2, q1)) return true;
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false;
}

// // Function to check for collisions between moving objects
void checkCollisions(vector<movingObject>& objects) {
    for (size_t i = 0; i < objects.size(); ++i) {
        for (size_t j = i + 1; j < objects.size(); ++j) {
            const int* a_start = objects[i].getOldLocation();
            const int* a_end   = objects[i].getLocation();
            const int* b_start = objects[j].getOldLocation();
            const int* b_end   = objects[j].getLocation();

            if (doLinesIntersect(a_start, a_end, b_start, b_end)) {
                // Check if they only touch at a start point
                bool touchesAtStartOnly =
                    isSamePoint(a_start, b_start) ||
                    isSamePoint(a_start, b_end)   ||
                    isSamePoint(a_end, b_start);

                // Check if the segments are collinear
                bool collinear =
                    areCollinear(a_start, a_end, b_start) &&
                    areCollinear(a_start, a_end, b_end);

                if (touchesAtStartOnly && !collinear) {
                    continue; // Valid case — not a collision
                }

                // Otherwise, it's a collision
                objects[i].setIsAlive();
                objects[j].setIsAlive();
            }
        }
    }
}

string gameManager::makeAllMoves(vector<movingObject>& objects) {
    for (size_t i = 0; i < objects.size(); ++i) {
        (*gameBoard)[objects[i].getOldLocation()[0]][objects[i].getOldLocation()[1]][1] = nullptr;
        if(!objects[i].getIsAlive()) {
            objects.erase(objects.begin() + i);
            --i;
            if(typeid(objects[i]) == typeid(bullet)){



                // TODO: erase the bullet from bullets in the air and currMovingObjects and use distractor





            }
        }
        else {
            (*gameBoard)[objects[i].getLocation()[0]][objects[i].getLocation()[1]][1] = &objects[i];
            if(typeid(objects[i]) == typeid(tank) && typeid((*gameBoard)[objects[i].getLocation()[0]][objects[i].getLocation()[1]]) == typeid(mine)) {
                objects[i].setIsAlive();
                (*gameBoard)[objects[i].getOldLocation()[0]][objects[i].getOldLocation()[1]][1] = nullptr;
                (*gameBoard)[objects[i].getOldLocation()[0]][objects[i].getOldLocation()[1]][0] = nullptr;
                objects.erase(objects.begin() + i);
                --i;


                // TODO: use distractor on the mine




            }
            if(typeid(objects[i]) == typeid(bullet) && typeid((*gameBoard)[objects[i].getLocation()[0]][objects[i].getLocation()[1]]) == typeid(wall)) {
                objects[i].setIsAlive();
                (*gameBoard)[objects[i].getOldLocation()[0]][objects[i].getOldLocation()[1]][1] = nullptr;
                objects.erase(objects.begin() + i);
                --i;



                // TODO: check if the wall still exist and if not use distractor on it and delete from gameboard




            }

        }
    }
}

void gameManager::playGame(){
    bool gameOver = false;
    move p1Move, p2Move;
    currMovingObjects.push_back(*tanks[0]);
    currMovingObjects.push_back(*tanks[1]);
    while(!gameOver && noBulletsCnt > 0){
        if(!(tanks[0]->canShot()) && !(tanks[1]->canShot())) {
            noBulletsCnt--;
        }
        moveBullets();
        if(isOddTurn){
//            p1Move = tanks[0].play();
//            p2Move = tanks[1].play();
//            bool canP1MakeMove = canMakeMove(tanks[0], p1Move);
//            bool canP2MakeMove = canMakeMove(tanks[1], p2Move);
        }
        checkCollisions(currMovingObjects);
        makeAllMoves(currMovingObjects);

        if(!(tanks[0]->getIsAlive()) || !(tanks[1]->getIsAlive())) {
            gameOver = true;
        }
    }
    if(  tanks[0]->getIsAlive()  &&  tanks[1]->getIsAlive()  && (noBulletsCnt == 0)) {
        // it's a tie
    }

    if(!(tanks[0]->getIsAlive()) && !(tanks[1]->getIsAlive())) {
        // it's a tie
    }
    else if(!(tanks[0]->getIsAlive())) {
        // player 2 won
    }
    else if{
        // player 1 won
    }

}

//        TODO: Make a log file.
//        TODO: Implement above functions
//        TODO: use distractor where needed








gameManager::gameManager(const std::string &filename) {
    createMap(filename);
    noBulletsCnt = 40;
}

gameManager::~gameManager() {
    for (int i = 0; i < numOfRows; ++i) {
        for (int j = 0; j < numOfCols; ++j) {
            for (int k = 0; k < 2; ++k) {
                if (!(*gameBoard)[i][j][k]){
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
    // 1. **Point and MovingObject Structures**: Represent points and moving objects with start and end positions.
    // 2. **Line Intersection Check**: The `doLinesIntersect` function uses orientation and collinearity checks to determine if two line segments intersect.
    // 3. **Collision Checker**: The `checkCollisions` function iterates through all pairs of moving objects and checks for intersections.
    //
    // You can use this function by passing a list of moving objects with their planned movements.