#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include "matrixObject.h"
#include "movingObject.h"
#include "unmovingObject.h"
#include "bullet.h"
#include "move.h"
#include "tank.h"
#include "mine.h"
#include "wall.h"
#include "player.h"
#include "orientation.h"
#include <string>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <array>

using namespace std;

// A function used for writing to files(e.g., error logs) without necessarily opening them beforehand.
// The function return true if the file was opened successfully and the message was written to it.
inline bool writeToFile(const string &message, const string &filename)
{
    ofstream fileToWrite(filename, ios::app);
    if (fileToWrite.is_open())
    {
        fileToWrite << message << endl;
        fileToWrite.close();
        return true;
    }
    else
    {
        cerr << "Error: Could not open the error file." << endl;
        return false;
    }
}

class gameManager
{
private:
    int numOfRows;
    int numOfCols;
    int turns;
    int noBulletsCnt;
    bool isOddTurn;
    vector<vector<array<matrixObject*, 3>>> *gameBoard;
    vector<bullet> bullets; // Vector to store bullets in the air
    array<tank*, 2> tanks; // Vector to store tanks on the board
    vector<movingObject> currMovingObjects;

    bool makeAllMoves(vector<movingObject>& objects);
    bool canMakeMove(tank& tankChoseTheMove, enum move moveChosen);
    bool makeTankMoves(vector<tank>& tanks);
    bool checkCollisions(vector<movingObject>& objects);



public:
    gameManager(const string &filename);
    ~gameManager();
    bool createMap(const string &filename);
    void playGame();
    void moveBullets();
};

#endif // GAME_MANAGER_H

/*

Need to add:

- player class, player1 and player2

- player1 and player2 inherit the player class. inherit since every player needs two different algorithms

- figure out how to implement the game loop

- save an array of bullets in the air

- either turn counter, or a variable to say if on even or odd turn(for overflow considerations)

- save an array of tanks on the board

- figure out how to create an interface between the player algorithms and the gameManager class

- save a matrix of the game board in the gameManager class

*/