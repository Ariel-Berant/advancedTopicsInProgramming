#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include "matrixObject.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class gameManager
{
private:

    int turns;
    int noBulletsCnt;
    bool isEvenTurn;
    std::vector<std::vector<matrixObject>> gameBoard; // 2D vector to represent the game board
    std::vector<movingObject> bullets; // Vector to store bullets in the air
    std::vector<movingObject> tanks; // Vector to store tanks on the board

public:
    gameManager(/* args */);
    ~gameManager();
};

gameManager::gameManager(/* args */)
{
}

gameManager::~gameManager()
{
}

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