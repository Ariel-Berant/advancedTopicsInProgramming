#include "gameManager.h"

#define INP_ERR_FILE "input_errors.txt"
#define GAME_LOG_FILE "log_file.txt"
#define isCritErr(bool) { \
if(!bool){return false}                 \
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

    if (getline(file, line)) {
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

    gameBoard = new vector<vector<array<matrixObject *, 2>>>(rows, vector<array<matrixObject *, 2>>(cols));
    tanks[0] = nullptr;
    tanks[1] = nullptr;

    while (std::getline(file, line) && !criticalErr) {
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
                    isCritErr(writeToFile("Error: unrecognized character '"+ch+"' in the map file.", INP_ERR_FILE));
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

    file.close();
    return true;

}

// eligible

void gameManager::playGame(){
    bool gameOver = false;
    move p1Move, p2Move;
    while(!gameOver && noBulletsCnt > 0){
//        moveBullets();
//        checkBulletsCollisions();
        if(isOddTurn){
//            p1Move = tanks[0].play();
//            p2Move = tanks[1].play();
//            bool canP1MakeMove = canMakeMove(tanks[0], p1Move);
//            bool canP2MakeMove = canMakeMove(tanks[1], p2Move);
//            checkTanksCollisions(p1Move, canP1MakeMove, p2Move, canP2MakeMove );
//            makeMove(tanks[0], p1Move, canP1MakeMove);
//            makeMove(tanks[1], p2Move, canP2MakeMove);
        }
//        makeAllMoves();
//        checkAllCollisions();
//        TODO: Create an array of moves to be made, and execute them all at end of iteration.
//        TODO: Implement above functions
    }
}

/*
 *
 * ##       21
 * 12 --->  ##
 *
 *
 */

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
            delete (*gameBoard)[i][j];
            (*gameBoard)[i][j] = nullptr;
        }
        delete (*gameBoard)[i];
        (*gameBoard)[i] = nullptr;
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