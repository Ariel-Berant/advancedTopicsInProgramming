#include "OurSattelliteView.h"

using namespace UserCommon_0000;

OurSattelliteView::OurSattelliteView(const vector<vector<array<shared_ptr<matrixObject>, 3>>> gBoard, size_t numOfCols, size_t numOfRows, size_t tankX, size_t tankY):
 gameBoard(gBoard), numOfRows(numOfRows), numOfCols(numOfCols), tankX(tankX), tankY(tankY){
    // Initialize the game board with the provided shared pointer
    // The game board is a 2D vector of arrays, where each array contains three shared pointers to matrixObject.
    // The first element represents the wall, the second element represents the tank, and the third element represents the bullet
    // Create a board with the same dimensions as gBoard, with all nullptr values
    gameBoard = vector<vector<array<shared_ptr<matrixObject>, 3>>>(
        gBoard.size(),
        vector<array<shared_ptr<matrixObject>, 3>>(
            gBoard[0].size(),
            {nullptr, nullptr, nullptr}  // Initialize all 3 elements to nullptr
        )
    );

    for (size_t i = 0; i < gBoard.size(); i++)
    {
        for (size_t j = 0; j < gBoard[i].size(); j++)
        {
            if (gBoard[i][j][1] != nullptr)
            {
                if (gBoard[i][j][1]->getType() == P1T)
                {
                    gameBoard[i][j][1] = std::make_unique<matrixObject>(0,0,P1T);
                }
                else if (gBoard[i][j][1]->getType() == P2T)
                {
                    gameBoard[i][j][1] = std::make_unique<matrixObject>(0,0,P2T);
                }
                else // Bullet
                {
                    gameBoard[i][j][1] = std::make_unique<matrixObject>(0,0,B);
                }
            }
            if (gBoard[i][j][0] != nullptr)
            {
                if (gBoard[i][j][0]->getType() == W)
                {
                    gameBoard[i][j][0] = std::make_unique<matrixObject>(0,0,W);
                }
                else if (gBoard[i][j][0]->getType() == M)
                {
                    gameBoard[i][j][0] = std::make_unique<matrixObject>(0,0,M);
                }
            }
        }
    }
    
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