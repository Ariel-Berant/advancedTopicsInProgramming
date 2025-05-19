#include "playerTank.h"

void Player1TankAlgorithm::updateTurn() {
    if(turnsUntilNextShot > 0){
        turnsUntilNextShot--;
    }
    if(calcMoveRound < 7){
        calcMoveRound++;
    } else{
        calcMoveRound = 0;
    }
}

ActionRequest Player1TankAlgorithm::getAction() {
    // Implement the logic to get the action for Player 1's tank
    // This is a placeholder implementation
    return ActionRequest();
}

void Player1TankAlgorithm::updateBattleInfo(BattleInfo& info) {
    // Implement the logic to update the battle information for Player 1's tank
    // This is a placeholder implementation
}

Player1TankAlgorithm::Player1TankAlgorithm(int row, int col, orientation orient)  : PlayerTankAlgorithm(row, col, orient, P1T) {}

bool Player1TankAlgorithm::checkIfOnSameLine(const int *otherLoc) const {
    if(abs(location[0] - otherLoc[0]) == abs(location[1] - otherLoc[1])){ 
        // check if both tanks are on the same diagonal - linear function, bias cancels out, and slope is 1
        return true;
    }
    if (location[0] == otherLoc[0] || location[1] == otherLoc[1]) { //check if both tanks are on the same row or column
        return true;
    }
    return false;
}


objMove Player1TankAlgorithm::play(const vector<vector<array<shared_ptr<matrixObject>, 3>>> &gameBoard, const int *otherLoc, int numOfCols,
                     int numOfRows) {
    objMove currAction;

    // First check for immediate danger\moves - if the other tank is in the same line, or if threatened. Otherwise, play normally

    // Check if current location is safe
    if(!isSafe(location[0], location[1], gameBoard, numOfCols, numOfRows, 1) || !isSafe(location[0], location[1], gameBoard, numOfCols, numOfRows, 2)
            || !isSafe(location[0], location[1], gameBoard, numOfCols, numOfRows, 3)){
        // If not safe, find a safe adjacent location
        pair<objMove, int> nextMove = findAdjSafe(gameBoard, numOfCols, numOfRows);
        if(nextMove.first != noAction){
            return nextMove.first;
        }
    }

    // Check if the other tank is in the same line
    if (checkIfOnSameLine(otherLoc)) {// just need to change rotation and shoot
        int targetOrientation = calculateTargetOrientation(otherLoc[0], otherLoc[1]);
        objMove nextMove = determineNextMove(orient, targetOrientation).first;
        if(nextMove == moveForward && canShoot()){
            return shoot;
        }
        else if(nextMove != moveForward){
            return nextMove;
        }
    }

    if (moves.empty() || calcMoveRound == 0) {
        moves = playCalc(gameBoard, otherLoc, numOfCols, numOfRows);
    }

    // If first move is noAction, couldn't find a path to the other tank.
    // So, try whatever is available in the same row or column, to get closer to the other tank.
    if(!moves.empty() && moves[0] == noAction){
        // Try to be in the same row as the other tank
        int locationToCheck[2] = {otherLoc[0] , location[1]};
        moves = playCalc(gameBoard, locationToCheck, numOfCols, numOfRows);
        if(moves.empty() || moves[0] == noAction){
            // Try to be in the same column as the other tank
            locationToCheck[0] = location[0];
            locationToCheck[1] = otherLoc[1];
            moves = playCalc(gameBoard, locationToCheck, numOfCols, numOfRows); 
        }
    }
    
    currAction = moves[0];
    moves.erase(moves.begin());
    return currAction;
}

vector<objMove> Player1TankAlgorithm::playCalc(const vector<vector<array<shared_ptr<matrixObject>, 3>>> &gameBoard, const int *tank2Loc,
                                 int numOfCols, int numOfRows) {

    vector<vector<bool>> visited(numOfCols, vector<bool>(numOfRows, false));
    queue<pair<pair<int,int>, vector<array<int, 3>>>> q;

    q.push({{location[0], location[1]}, {}}); // Start position
    visited[location[0]][location[1]] = true;

    while (!q.empty()) {
        auto front = q.front();
        auto current = front.first;
        auto path = front.second;

        q.pop();

        int col = current.first, row = current.second;

        // If we reach the target, return the path
        if (col == tank2Loc[0] && row == tank2Loc[1]) {
            int shots = shotsLeft, sinceShot = turnsUntilNextShot;
            vector<objMove> moves, currRotations;
            orientation currOrient = orient;
            for (const auto& coords : path) {
                currRotations = getRotations(currOrient, (orientation)coords[2]);
                for (objMove move : currRotations){
                    if (shots > 0 && sinceShot == 0
                    && canSeeOtherTank(tank2Loc, numOfCols, numOfRows)) {
                        moves.push_back(shoot);
                        shots--;
                        sinceShot = 4;
                        moves.push_back(move);
                    }
                    else {
                        moves.push_back(move);
                        sinceShot--;
                    }
                    currOrient = (orientation)coords[2];
                }
            }
            return moves;
        }

        // Explore neighbors
        vector<array<int, 3>> directions = {{0, -1, U}, {0, 1, D}, {-1, 0, L}, {1, 0, R},
                                             {-1, -1, UL}, {1, -1, UR}, {-1, 1, DL}, {1, 1, DR}};
        for (array<int, 3> coords: directions) {
            int nRow = (row + coords[1] + numOfRows) % numOfRows; // Wrap around rows
            int nCol = (col + coords[0] + numOfCols) % numOfCols; // Wrap around cols

            if (!visited[nCol][nRow]
            && isSafe(nCol, nRow, gameBoard, numOfCols, numOfRows, (int)path.size() + 1)) {
                vector<array<int, 3>> newPath = path;
                array<int, 3> newCoords = {nCol, nRow, coords[2]};
                newPath.push_back(newCoords); // Add new direction to path
                q.push({{nCol, nRow}, newPath});
            }
            visited[nCol][nRow] = true;
        }
    }

    return handleSurrounded(gameBoard, tank2Loc); // Return empty path if no valid path found
}

vector<objMove> Player1TankAlgorithm::handleSurrounded(const vector<vector<array<shared_ptr<matrixObject>, 3>>> &gameBoard, const int *tank2Loc) const {
    vector<objMove> currMoves;
    if (isSurrounded(gameBoard, location)) {
        // Handle the case when the tank is surrounded
        // For example, you can rotate or move in a specific direction
        currMoves = {shoot, noAction, noAction, noAction, noAction, shoot};
    } else {
        if (isSurrounded(gameBoard, tank2Loc)) {
            // Handle the case when the other tank is surrounded
            // For example, you can rotate or move in a specific direction
            vector<array<int, 3>> directions = {{0, -1, U}, {0, 1, D}, {-1, 0, L}, {1, 0, R},
                                             {-1, -1, UL}, {1, -1, UR}, {-1, 1, DL}, {1, 1, DR}};
            for (array<int, 3> coords: directions) {
                int nRow = (location[1] + coords[1] + gameBoard[0].size()) % gameBoard[0].size(); // Wrap around rows
                int nCol = (location[0] + coords[0] + gameBoard.size()) % gameBoard.size(); // Wrap around cols

                if (isSafe(nCol, nRow, gameBoard, gameBoard.size(), gameBoard[0].size(), 1)) {
                    vector<objMove> currRotations = getRotations(orient, (orientation)coords[2]);
                    for (objMove move : currRotations) {
                        currMoves.push_back(move);
                    }
                    currMoves.push_back(shoot);
                    break;
                }
            }
        }
        if (currMoves.size() == 0) {
            // If no moves were added, you can add a default action
            currMoves = vector<objMove>(5, noAction);
        }
    }
    return currMoves;
}