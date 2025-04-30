#include "playerTank.h"

void p1Tank::updateTurn() {
    if(turnsUntilNextShot > 0){
        turnsUntilNextShot--;
    }
    if(calcMoveRound < 7){
        calcMoveRound++;
    } else{
        calcMoveRound = 0;
    }
}

p1Tank::p1Tank(int row, int col, orientation orient)  : tank(row, col, orient, P1T) {}

objMove p1Tank::play(const vector<vector<array<matrixObject *, 3>>> &gameBoard, const int *otherLoc, int numOfCols,
                     int numOfRows) {
    objMove currAction;
    if (moves.empty() || calcMoveRound == 0) {
        moves = playCalc(gameBoard, otherLoc, numOfRows, numOfCols);
    }
    currAction = moves[0];
    moves.erase(moves.begin());
    return currAction;
}

vector<objMove> p1Tank::playCalc(const vector<vector<array<matrixObject *, 3>>> &gameBoard, const int *tank2Loc,
                                 int numOfRows, int numOfCols) {

    vector<vector<bool>> visited(numOfRows, vector<bool>(numOfCols, false));
    queue<pair<pair<int,int>, vector<array<int, 3>>>> q;

    q.push({{location[0], location[1]}, {}}); // Start position
    visited[location[0]][location[1]] = true;

    while (!q.empty()) {
        auto front = q.front();
        auto current = front.first;
        auto path = front.second;

        q.pop();

        int row = current.first, col = current.second;

        // If we reach the target, return the path
        if (row == tank2Loc[0] && col == tank2Loc[1]) {
            int shots = shotsLeft, sinceShot = turnsUntilNextShot;
            vector<objMove> moves, currRotations;
            orientation currOrient = orient;
            for (const auto& coords : path) {
                currRotations = getRotations(currOrient, (orientation)coords[2]);
                for (objMove move : currRotations){
                    if (shots > 0 && sinceShot == 0
                    && canSeeOtherTank(tank2Loc, gameBoard, numOfRows, numOfCols)) {
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
        vector<array<int, 3>> directions = {{-1, 0, U}, {1, 0, D}, {0, -1, L}, {0, 1, R},
                                             {-1, -1, UL}, {-1, 1, UR}, {1, -1, DL}, {1, 1, DR}};
        for (array<int, 3> coords: directions) {
            int nRow = (row + coords[0] + numOfRows) % numOfRows; // Wrap around rows
            int nCol = (col + coords[1] + numOfCols) % numOfCols; // Wrap around cols

            if (!visited[nRow][nCol]
            && isSafe(nRow, nCol, gameBoard, numOfCols, numOfRows, (int)path.size() + 1)) {
                vector<array<int, 3>> newPath = path;
                array<int, 3> newCoords = {nRow, nCol, coords[2]};
                newPath.push_back(newCoords); // Add new direction to path
                q.push({{nRow, nCol}, newPath});
            }
            visited[nRow][nCol] = true;
        }
    }

    return handleSurrounded(gameBoard, tank2Loc); // Return empty path if no valid path found
}


// TODO: Add handleSurrounded function

vector<objMove> p1Tank::handleSurrounded(const vector<vector<array<matrixObject *, 3>>> &gameBoard, const int *tank2Loc) {
    vector<objMove> currMoves;
    if (isSurrounded(gameBoard, location)) {
        // Handle the case when the tank is surrounded
        // For example, you can rotate or move in a specific direction
        currMoves = {shoot, noAction, noAction, noAction, noAction, shoot};
    } else {
        if (isSurrounded(gameBoard, tank2Loc)) {
            // Handle the case when the other tank is surrounded
            // For example, you can rotate or move in a specific direction
            vector<array<int, 3>> directions = {{-1, 0, U}, {1, 0, D}, {0, -1, L}, {0, 1, R},
                                             {-1, -1, UL}, {-1, 1, UR}, {1, -1, DL}, {1, 1, DR}};
            for (array<int, 3> coords: directions) {
                int nRow = (location[0] + coords[0] + gameBoard.size()) % gameBoard.size(); // Wrap around rows
                int nCol = (location[1] + coords[1] + gameBoard[0].size()) % gameBoard[0].size(); // Wrap around cols

                if (isSafe(nRow, nCol, gameBoard, gameBoard[0].size(), gameBoard.size(), 1)) {
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