#include "playerTank.h"

p1Tank::p1Tank(int x, int y, orientation orient)  : tank(x, y, orient, P1T) {}

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
    objMove currAction = noAction;

    vector<vector<bool>> visited(numOfRows, vector<bool>(numOfCols, false));
    queue<pair<pair<int,int>, vector<array<int, 3>>>> q;

    q.push({{location[0], location[1]}, {}}); // Start position
    visited[location[0]][location[1]] = true;

    while (!q.empty()) {
        auto front = q.front();
        auto current = front.first;
        auto path = front.second;

        q.pop();

        int x = current.first, y = current.second;

        // If we reach the target, return the path
        if (x == tank2Loc[0] && y == tank2Loc[1]) {
            int shots = shotsLeft, sinceShot = turnsUntilNextShot;
            vector<objMove> moves, currRotations;
            orientation currOrient = orient;
            for (const auto& coords : path) {
                currRotations = getRotations(currOrient, (orientation)coords[2]);
                for (objMove move : currRotations){
                    if (shots > 0 && sinceShot == 0
                    && canSeeOtherTank(tank2Loc, gameBoard, numOfRows, numOfCols)) {
                        moves.emplace_back(shoot);
                        shots--;
                        sinceShot = 4;
                        moves.emplace_back(move);
                    }
                    else {
                        moves.emplace_back(move);
                        sinceShot--;
                    }
                }
                moves.emplace_back(moveForward);
            }
            return moves;
        }

        // Explore neighbors
        vector<array<int, 3>> directions = {{-1, 0, U}, {1, 0, D}, {0, -1, L}, {0, 1, R},
                                             {-1, -1, UL}, {-1, 1, UR}, {1, -1, DL}, {1, 1, DR}};
        for (array<int, 3> coords: directions) {
            int nx = (x + coords[0] + numOfCols) % numOfCols; // Wrap around rows
            int ny = (y + coords[1] + numOfRows) % numOfRows; // Wrap around cols

            if (!visited[nx][ny]
            && isSafe(nx, ny, gameBoard, numOfCols, numOfRows, (int)path.size() + 1)) {
                vector<array<int, 3>> newPath = path;
                array<int, 3> newCoords = {nx, ny, coords[2]};
                newPath.emplace_back(newCoords); // Add new direction to path
                q.push({{nx, ny}, newPath});
            }
            visited[nx][ny] = true;
        }
    }

    return vector<objMove>(10, noAction); // Return empty path if no valid path found
}


