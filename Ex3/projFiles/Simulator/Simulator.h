#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <dlfcn.h>
#include <sstream>
#include <utility>
#include "AlgorithmRegistrar.h"
#include "GameManagerRegistrar.h"
#include "../UserCommon/OurSattelliteView.h"
#include "../UserCommon/MatrixObject.h"
#include "../UserCommon/UnmovingObject.h"
#include "../UserCommon/ObjectType.h"
#include "../UserCommon/MovingObject.h"
#include "../UserCommon/Orientation.h"
#include "../common/AbstractGameManager.h"
#include "../common/TankAlgorithm.h"
#include "../common/Player.h"
#include "ThreadPool.h"

using namespace UserCommon_0000;

using std::string, std::vector, std::unique_ptr, std::shared_ptr, std::array, std::pair, std::endl;
namespace Simulator_0000
{
    struct InputError
        {
            bool errorOccured;
            string message;
            InputError() : errorOccured(false), message("") {};
            InputError(const string& msg) : errorOccured(true), message(msg) {};
        };

    class Simulator
    {
    private:
        struct Config
        {
            int runType = 0; // 1 for competition, 2 for comparative, 
            bool verbose = false;
            string mapPath;
            string mapsFolderPath;
            string gameManagerPath;
            string gameManagerFolderPath;
            string algorithm1Path;
            string algorithm2Path;
            string algorithmsFolderPath;
            int threadsNum = 1;
            bool hasUnsupportedArgs = false;
        };


        Config config;

        struct MapData
        {
            string mapName;
            size_t maxTurns;
            size_t numShells;
            size_t mapHeight;
            size_t mapWidth;
            OurSattelliteView map;

            MapData(string mapName, size_t maxTurns, size_t numShells, size_t mapHeight, size_t mapWidth, OurSattelliteView map)
                : mapName(std::move(mapName)), maxTurns(maxTurns), numShells(numShells), mapHeight(mapHeight), mapWidth(mapWidth), map(std::move(map))
            {}

            MapData(MapData&& other) : map(std::move(other.map)){
                mapName = std::move(other.mapName);
                maxTurns = other.maxTurns;
                numShells = other.numShells;
                mapHeight = other.mapHeight;
                mapWidth = other.mapWidth;
            }
        };

        struct runObj
        {
            shared_ptr<Player> player1;
            shared_ptr<Player> player2;
            TankAlgorithmFactory tankFactory1;
            TankAlgorithmFactory tankFactory2;
            shared_ptr<AbstractGameManager> gameManager;
            shared_ptr<MapData> mapData;
            string gameManagerName;
            string algo1Name;
            string algo2Name;

            runObj(shared_ptr<Player> p1, shared_ptr<Player> p2, TankAlgorithmFactory t1, TankAlgorithmFactory t2, shared_ptr<AbstractGameManager> gm, shared_ptr<MapData> md, string gmName, string a1Name, string a2Name)
                : player1(std::move(p1)), player2(std::move(p2)), tankFactory1(std::move(t1)), tankFactory2(std::move(t2)), gameManager(std::move(gm)), mapData(std::move(md)), gameManagerName(std::move(gmName)), algo1Name(std::move(a1Name)), algo2Name(std::move(a2Name))
            {};

            ~runObj(){
            };
        };

        std::string fileToPrintPath;
        vector<shared_ptr<MapData>> mapsData;
        vector<string> algos;
        vector<string> gameManagers;
        vector<void*> handles;
        vector<runObj> runObjects;
        vector<std::future<std::pair<GameResult, string>>> results;
        vector<pair<GameResult, vector<string>>> comparativeGrouped;
        vector<pair<string, size_t>> competitionGrouped;

        void printInput();
        void loadConfigFromInput(int argc, char const *argv[]);
        InputError validateInput(int argc, char const *argv[]);
        bool checkConfig();
        InputError getNamesComparative();
        InputError getNamesCompetition();
        InputError loadMapsData();
        void loadAlgorithms();
        void loadGameManagers();
        void loadRunObjectsComparative();
        void loadRunObjectsCompetition();
        void sendRunObjectsToThreadPool(ThreadPool& threadPool);
        void runRegularRunObjects();
        void sortResultsComparative();
        void printResultsComparative();
        void sortResultsCompetition();
        void printResultsCompetition();
        static Simulator simulator;
        vector<vector<array<shared_ptr<matrixObject>, 3>>> createSatView(const std::string& filePath, size_t numOfCols, size_t numOfRows);
        string parseGameResultReason(const GameResult& gr, int mapIndex) const;
        bool loadAndCheckAll(int argc, char const *argv[]);
        bool unloadAll();
        pair<string, array<size_t, 4>> parseMapLine(const string& filename);
        size_t parseRowInfo(const string line, const string description, int rowNum);
        void addUnmovingObjectToMap(vector<vector<array<shared_ptr<matrixObject>, 3>>>& gameBoard, char UnmovingObjectType, size_t currCol, size_t currRow);
        void addTankToMap(vector<vector<array<shared_ptr<matrixObject>, 3>>>& gameBoard, size_t playerNum, size_t currCol, size_t currRow);

        void comparativeRun();
        void competitionRun();

    public:
        void simulate(int argc, char const *argv[]);
        Simulator() = default;
        ~Simulator() {};
    };

}
