#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include "MatrixObject.h"
#include "MovingObject.h"
#include "UnmovingObject.h"
#include "Bullet.h"
#include "Move.h"
#include "PlayerTankAlgorithm.h"
#include "Mine.h"
#include "Wall.h"
#include "Orientation.h"
#include "PlayerTank.h"
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
#include "OurPlayer.h"
#include "OurSattelliteView.h"
#include "PseudoTank.cpp"
/*
 * Includes in matrixObject.h:
 *
 * #include <vector>
 * #include <array>
 * #include <queue>
 * #include <unordered_set>
 * #include <string>
 * #include <cmath>
 * #include <memory>
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
    int numOfP1TanksLeft;
    int numOfP2TanksLeft;
    int numOfP1Tanks;
    int numOfP2Tanks;
    int numOfBulletsLeft;
    vector<string> printToLogVector; // Vector to store messages to print to the log file
    unique_ptr<vector<vector<array<shared_ptr<matrixObject>, 3>>>> gameBoard;
    vector<shared_ptr<bullet>> bullets; // Vector to store bullets in the air
    vector<shared_ptr<PseudoTank>> tanks; // Vector to store tanks on the board
    vector<shared_ptr<movingObject>> currMovingObjects;
    shared_ptr<Player> player1;
    shared_ptr<Player> player2;
    
    bool makeAllMoves();
    bool canMakeMove(PseudoTank& tankChoseTheMove, enum ActionRequest moveChosen);
    void getMovesFromTanks();
    bool checkCollisions();
    bool getRowsAndColsFromFile(const string &filename);
    void printSummeryToOurLog();
    bool isValidFile(const string &filename);
    void printCollisionsToLog(const movingObject &object1, const movingObject &object2) const;
    void moveBullets();
    bool gameManager::parseGameInfo(const string line, const string description, int rowNum);
    void updateAboutNewDstroyedTanks();
    void printLastTurnToLog();
    void printGameResultToLog();
    void gameManager::printToOurLogGameResult();
    bool addTankToMap(int playerNum, int row, int col, TankAlgorithmFactory &tankFactory);
    bool addUnmovingObjectToMap(char UnmovingObjectType, int currCol, int currRow);
    void completeColumns(int currCol, int currRow);
    void completeRows(int currRow);
    void waitingforBackwordMove(ActionRequest tanksMove, int i);
    void moveForwardMove(bool tankCanMove ,ActionRequest tanksMove, int i);
    void moveBackwardMove(bool tankCanMove ,ActionRequest tanksMove, int i);
    void shootMove(bool tankCanMove, ActionRequest tanksMove, int i);
    void getTheIthTankMove(int i, ActionRequest &tanksMove);
    void actualymakeMoves();
    void dealWithDamagedUnmovingObject(int i, const int objectNewCol, const int objectNewRow);
    bool createMap(const string &filename, TankAlgorithmFactory &tankFactory);
    orientation gameManager::calculateNewOrientation(ActionRequest &tanksMove, int i);


public:
    gameManager(const string &filename);
    ~gameManager();
    bool initializeGame(const string &filename, TankAlgorithmFactory &tankFactory, PlayerFactory &playerFactory);
    void playGame();
};

#endif // GAME_MANAGER_H