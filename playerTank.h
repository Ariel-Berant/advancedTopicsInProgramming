#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include "tank.h"


class p1Tank : public tank {
public:
    p1Tank(int x, int y, orientation orient);
    objMove play(const vector<vector<array<matrixObject *, 3>>> &gameBoard, const int otherLoc[2], int numOfCols, int numOfRows) override;
    vector<objMove> playCalc(const vector<vector<array<matrixObject *, 3>>> &gameBoard, const int tank2Loc[2], int numOfRows, int numOfCols);
};

class p2Tank : public tank
{
public:
    p2Tank(int x, int y, orientation orient);
    objMove play(const vector<vector<array<matrixObject*, 3>>>& gameBoard, const int otherLoc[2], int numOfCols, int numOfRows) override;
    pair<int, int> getNeighborPointGivenOrient(int orient);
    pair<objMove, int>  determineNextMove(int currentOrientation, int targetOrientation, int targetX, int targetY);
    array<int,4> searchForBullets(const vector<vector<array<matrixObject *, 3>>> &gameBoard, int numOfCols, int numOfRows, int inRows, int inCols);
};

#endif //PLAYERTANK_H
