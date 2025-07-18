#include "PlayerTank.h"

void Player1TankAlgorithm::updateTurn() {
    if(turnsUntilNextShot > 0){
        turnsUntilNextShot--;
    }
    if(calcMoveRound < 4){
        calcMoveRound++;
    } else{
        calcMoveRound = 0;
    }
}

ActionRequest Player1TankAlgorithm::getAction() {
    return play()
    ;
}

Player1TankAlgorithm::Player1TankAlgorithm(int row, int col, orientation orient)  : PlayerTankAlgorithm(row, col, orient, P1T) {}

ActionRequest Player1TankAlgorithm::playHelper(int numOfRows, int numOfCols, const int *closestEnemyLoc) {
    moveTankBullets(numOfCols, numOfRows); // Move the tank's bullets
    if (!moves.empty() && moves[0] == ActionRequest::GetBattleInfo) {
        moves.erase(moves.begin()); // Remove GetBattleInfo from the moves list
        return ActionRequest::GetBattleInfo; // Return GetBattleInfo action
    }

    // Check if current location is safe
    if(!isSafe(location[0], location[1], numOfCols, numOfRows, 1, P1T) || !isSafe(location[0], location[1], numOfCols, numOfRows, 2, P1T)
            || !isSafe(location[0], location[1], numOfCols, numOfRows, 3, P1T)){
        // If not safe, find a safe adjacent location
        pair<ActionRequest, int> nextMove = findAdjSafe(numOfCols, numOfRows, P1T);
        if(nextMove.first != ActionRequest::GetBattleInfo){
            return nextMove.first;
        }
    }

    // Check if the other tank is in the same line
    if (checkIfOnSameLine(closestEnemyLoc)) {// just need to change rotation and shoot
        int targetOrientation = calculateTargetOrientation(closestEnemyLoc[0], closestEnemyLoc[1]);
        ActionRequest nextMove = determineNextMove(orient, targetOrientation).first;
        if(nextMove == ActionRequest::MoveForward && canShoot()){
            return ActionRequest::Shoot;
        }
        else if(nextMove != ActionRequest::MoveForward){
            return nextMove;
        }
    }

    return ActionRequest::DoNothing; // If no action was chosen, use this to ignore
}


ActionRequest Player1TankAlgorithm::play() {
    // First check for immediate danger\moves - if the other tank is in the same line, or if threatened. Otherwise, play normally
    currTurn++;
    if (currTurn == 1){
        return ActionRequest::GetBattleInfo; // First turn, just get battle info
    }

    const int numOfRows = tankBattleInfo->getGameBoard()[0].size();
    const int numOfCols = tankBattleInfo->getGameBoard().size(); 
    tankBattleInfo->setTurnsFromLastUpdate();
    ActionRequest currAction;
    const int closestEnemyLoc[2] = {tankBattleInfo->getClosestEnemyTankCol(), tankBattleInfo->getClosestEnemyTankRow()}; 

    currAction = playHelper(numOfRows, numOfCols, closestEnemyLoc);
    if (currAction != ActionRequest::DoNothing) {
        updateTankData(currAction, numOfCols, numOfRows); // Update tank data based on the action
        return currAction; // If an action was chosen, return it
    }

    if (moves.empty()) {
        if (tankBattleInfo->getTurnsFromLastUpdate() == 1){
            moves = playCalc(closestEnemyLoc, numOfCols, numOfRows);
        }
        else{
            moves.push_back(ActionRequest::GetBattleInfo); // If emptied moves, add GetBattleInfo action
        }
    }

    // If first move is noAction, couldn't find a path to the other tank.
    // So, try whatever is available in the same row or column, to get closer to the other tank.
    if(!moves.empty() && moves[0] == ActionRequest::GetBattleInfo){
        // Try to be in the same column as the other tank
        int locationToCheck[2] = {closestEnemyLoc[0] , location[1]};
        moves = playCalc(locationToCheck, numOfCols, numOfRows);
        if(moves.empty() || moves[0] == ActionRequest::GetBattleInfo){
            // Try to be in the same row as the other tank
            locationToCheck[0] = location[0];
            locationToCheck[1] = closestEnemyLoc[1];
            moves = playCalc(locationToCheck, numOfCols, numOfRows); 
        }
    }

    if (moves.empty()) {
        // In case we need to recalculate moves
        moves.push_back(ActionRequest::GetBattleInfo);
    }

    currAction = moves[0];
    moves.erase(moves.begin());
    updateTankData(currAction, numOfCols, numOfRows); // Update tank data based on the action
    return currAction;
}

vector<ActionRequest> Player1TankAlgorithm::playCalc(const int *tank2Loc,
                                 int numOfCols, int numOfRows) {

    vector<vector<bool>> visited(numOfCols, vector<bool>(numOfRows, false));
    queue<pair<pair<int,int>, vector<array<int, 3>>>> q;
    bool onSameLine;

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
            vector<ActionRequest> moves, currRotations;
            orientation currOrient = orient;
            for (const auto& coords : path) {
                currRotations = getRotations(currOrient, (orientation)coords[2]);
                for (ActionRequest move : currRotations){
                    onSameLine = checkIfOnSameLine(tank2Loc);
                    if (shots > 0 && sinceShot == 0
                    && canSeeOtherTank(tank2Loc, numOfCols, numOfRows)
                    && onSameLine
                    && !friendlyFireRisk(numOfCols, numOfRows, tank2Loc[0], tank2Loc[1])) {
                        moves.push_back(ActionRequest::Shoot);
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
            && isSafe(nCol, nRow,numOfCols, numOfRows, (int)path.size() + 1, P1T)) {
                vector<array<int, 3>> newPath = path;
                array<int, 3> newCoords = {nCol, nRow, coords[2]};
                newPath.push_back(newCoords); // Add new direction to path
                q.push({{nCol, nRow}, newPath});
            }
            visited[nCol][nRow] = true;
        }
    }

    return handleSurrounded(tank2Loc); // Return empty path if no valid path found
}

vector<ActionRequest> Player1TankAlgorithm::handleSurrounded(const int *tank2Loc) const {
    int numOfCols = tankBattleInfo->getGameBoard().size();
    int numOfRows = tankBattleInfo->getGameBoard()[0].size();
    vector<ActionRequest> currMoves;
    if (isSurrounded(location)) {
        // Handle the case when the tank is surrounded
        // For example, you can rotate or move in a specific direction
        currMoves = {ActionRequest::Shoot, ActionRequest::GetBattleInfo, ActionRequest::GetBattleInfo, ActionRequest::GetBattleInfo, ActionRequest::GetBattleInfo, ActionRequest::Shoot};
    } else {
        if (isSurrounded(tank2Loc)) {
            // Handle the case when the other tank is surrounded
            // For example, you can rotate or move in a specific direction
            vector<array<int, 3>> directions = {{0, -1, U}, {0, 1, D}, {-1, 0, L}, {1, 0, R},
                                             {-1, -1, UL}, {1, -1, UR}, {-1, 1, DL}, {1, 1, DR}};
            for (array<int, 3> coords: directions) {
                int nRow = (location[1] + coords[1] + numOfRows) % numOfRows; // Wrap around rows
                int nCol = (location[0] + coords[0] +numOfCols) % numOfCols; // Wrap around cols

                if (isSafe(nCol, nRow, numOfCols, numOfRows, 1, P1T)) {
                    vector<ActionRequest> currRotations = getRotations(orient, (orientation)coords[2]);
                    for (ActionRequest move : currRotations) {
                        currMoves.push_back(move);
                    }
                    currMoves.push_back(ActionRequest::Shoot);
                    break;
                }
            }
        }
        if (currMoves.size() == 0) {
            // If no moves were added, you can add a default action
            currMoves = vector<ActionRequest>(5, ActionRequest::GetBattleInfo);
        }
    }
    return currMoves;
}