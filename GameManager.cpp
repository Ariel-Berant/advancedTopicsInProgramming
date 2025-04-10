#include "gameManager.h"

using namespace std;

vector<vector<matrixObject>> gameManager::createMap(const std::string &filename) {
    ifstream file(filename);

    if (!file) {
        cerr << "Error: Could not open the file '" << filename << "'." << endl;
    }

    int rows, cols, currRow = 0, currCol = 0;
    string line;

    if (getline(file, line)) {
        istringstream iss(line);
        if (iss >> cols >> rows) {
//            numOfCols = cols;
//            numOfRows = rows;
//            TODO: What do we want to do here? What is this if statement being used for?
        }
        else {
            cerr << "Error: Failed to parse dimensions from line." << endl;
        }
    } else {
        cerr << "Error: Failed to read line from file." << endl;
    }




    while (std::getline(file, line)) {
        for (char ch : line) {
            switch (ch) {
//                case '#':
//                    gameBoard[currRow][currCol] = new wall(currRow, currCol);
//                    break;
//                case '1':
//                    gameBoard[currRow][currCol] = new tank(currRow, currCol, L);
//                    break;
//                case '2':
//                    gameBoard[currRow][currCol] = new tank(currRow, currCol, R);
//                    break;
//                case '@':
//                    gameBoard[currRow][currCol] = new mine(currRow, currCol);
//                    break;
//                default:
//                    cerr << "Error: unrecognized character '" << ch << "' in the map file." << endl;
//              TODO: Figure out how to initialize these vectors

            }
        }
    }

    file.close();

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