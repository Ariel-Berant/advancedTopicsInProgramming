#include "OurPlayer.h"


OurPlayer::OurPlayer(int player_index, size_t x, size_t y,
     size_t max_steps, size_t num_shells)
        : Player(player_index, x, y, max_steps, num_shells), // Call base class constructor
          player_index(player_index), x(x), y(y), max_steps(max_steps), num_shells(num_shells), lastTurnMapUpdated(0){


}



// need to continue the implementation of this function (deduce directions, find a way to update only if a turn has been made)
vector<vector<array<shared_ptr<matrixObject>, 3>>> OurPlayer::calculatePlayerGameBoard(SatelliteView& satellite_view, int playerIndex, PlayerTankAlgorithm& tank) {
    int currCol = 0, currRow = 0;
    char objType = satellite_view.getObjectAt(currCol, currRow);

    while (objType != ' ') {
        while (objType != ' '){
            objType = satellite_view.getObjectAt(currCol, currRow);
            playerGameBoard[currCol][currRow] = {nullptr, nullptr, nullptr};
            if (objType == ' ') {
                continue;
            }
            else if (objType == '#') {
                playerGameBoard[currCol][currRow][0] = make_shared<matrixObject>(W);
            }
            else if (objType == '@') {
                playerGameBoard[currCol][currRow][0] = make_shared<matrixObject>(M);
            }
            else if (objType == '1') {
                playerGameBoard[currCol][currRow][1] = make_shared<matrixObject>(P1T);
            }
            else if (objType == '2') {
                playerGameBoard[currCol][currRow][1] = make_shared<matrixObject>(P2T);
            }
            else if (objType == '*') {
                playerGameBoard[currCol][currRow][1] = make_shared<matrixObject>(B);
            }
            else{
                objectType oType = player_index == 1 ? P1T : P2T;
                playerGameBoard[currCol][currRow][1] = make_shared<matrixObject>(oType);




                tank.setLocation(currCol, currRow); // need to check if its forbidden to set the location of the tank here.
                // if it's not need to check how to set the location for every tank if more than one call this func in same turn





            }
            currRow++;
            objType = satellite_view.getObjectAt(currCol, currRow);
        }
        currCol++;
        currRow = 0;
        objType = satellite_view.getObjectAt(currCol, currRow);
    }
    return playerGameBoard;
}

