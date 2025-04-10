#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "tank.h"

class player{
    // The player class represents a player in the game, which can be either player1 or player2.

    tank playerTank; // The tank object representing the player's tank.

public:
    player(tank t): playerTank(t){};
    play(std::vector<std::vector<matrixObject*>> gameBoard){
        // This function is called to play a turn in the game.
        // It takes the game board, bullets, turn counter, and player number as parameters.
        // The function should implement the player's algorithm for making a move.
        // The implementation of this function is not provided in this snippet.

    }
};


#endif // PLAYER_H