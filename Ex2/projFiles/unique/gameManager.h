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
#include "orientation.h"
#include "playerTank.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include "TAFactory.cpp"
#include "../common/ActionRequest.h"
#include "../common/Player.h"
#include "../common/TankAlgorithm.h"
#include "../common/TankAlgorithmFactory.h"
#include "../common/PlayerFactory.h"
#include "../common/BattleInfo.h"

/*
 * Includes in matrixObject.h:
 *
 * #include <vector>
 * #include <array>
 * #include <queue>
 * #include <unordered_set>
 * #include <string>
 * #include <cmath>
 *
 */

using namespace std;

// A function used for writing to files(e.g., error logs) without necessarily opening them beforehand.
// The function return true if the file was opened successfully and the message was written to it.
inline bool writeToFile(const string &message, const string &filename)
{
    ofstream fileToWrite(filename, ios::app);
    if (fileToWrite.is_open())
    {
        try
        {
            fileToWrite << message << endl;
            fileToWrite.close();
            return true;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what();
        }
        
    }
    else
    {
        cerr << "Error: Could not open " << filename << " continuing the game without writing to it." << endl;
        return false;
    }
    return true;
}

class gameManager
{
private:
    string gameMapFileName;
    int numOfRows;
    int numOfCols;
    int turns;
    int maxTurns;
    int noBulletsCnt;
    int maxBullets;
    bool isOddTurn;
    int numOfWalls;
    int numOfMines;
    int numOfWallsDestroyed;
    int numOfMinesDestroyed;
    unique_ptr<vector<vector<array<shared_ptr<matrixObject>, 3>>>> gameBoard;
    vector<shared_ptr<bullet>> bullets; // Vector to store bullets in the air
    vector<shared_ptr<PlayerTankAlgorithm>> tanks; // Vector to store tanks on the board
    vector<shared_ptr<PlayerTankAlgorithm>> p1Tanks;
    vector<shared_ptr<PlayerTankAlgorithm>> p2Tanks;
    vector<shared_ptr<movingObject>> currMovingObjects;

    bool makeAllMoves();
    bool canMakeMove(PlayerTankAlgorithm& tankChoseTheMove, enum move moveChosen);
    void makeTankMoves();
    bool checkCollisions();
    bool getRowsAndColsFromFile(const string &filename);
    void printSummeryToLog();
    bool isValidFile(const string &filename);
    void printCollisionsToLog(const movingObject &object1, const movingObject &object2) const;
    void moveBullets();
    bool gameManager::parseGameInfo(const string line, const string description, int rowNum);

public:
    gameManager(const string &filename);
    ~gameManager();
    bool createMap(const string &filename, TankAlgorithmFactory &tankFactory);
    void playGame();
};

#endif // GAME_MANAGER_H