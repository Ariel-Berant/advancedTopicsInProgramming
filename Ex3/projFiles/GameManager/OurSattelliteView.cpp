#include "OurSattelliteView.h"

using namespace GameManager_0000;

OurSattelliteView::OurSattelliteView(const vector<vector<array<shared_ptr<matrixObject>, 3>>> gBoard, int numOfCols, int numOfRows, size_t tankX, size_t tankY):
 gameBoard(gBoard), numOfRows(numOfRows), numOfCols(numOfCols), tankX(tankX), tankY(tankY){
    // Initialize the game board with the provided shared pointer
    // The game board is a 2D vector of arrays, where each array contains three shared pointers to matrixObject.
    // The first element represents the wall, the second element represents the tank, and the third element represents the bullet

    
}

char OurSattelliteView::getObjectAt(size_t x, size_t y) const {
    if(x >= numOfCols || y >= numOfRows){
        return '&'; // Error: Out of bounds
    }
    if(x == tankX && y == tankY){
        return '%'; // Tank
    }
    if(gameBoard[x][y][1] != nullptr){
        if(gameBoard[x][y][1]->getType() == P1T){
            return '1'; // Player 1 tank
        }
        else if(gameBoard[x][y][1]->getType() == P2T){
            return '2'; // Player 2 tank
        }
        return '*'; // Bullet
    }
    if(gameBoard[x][y][0] != nullptr){
        if(gameBoard[x][y][0]->getType() == W){
            return '#'; // Wall
        }
        else if(gameBoard[x][y][0]->getType() == M){
            return '@'; // Mine
        }
    }
    return ' ' ;// Empty space
}