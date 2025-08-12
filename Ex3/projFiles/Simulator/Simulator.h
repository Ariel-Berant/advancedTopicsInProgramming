#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <dlfcn.h>
#include <sstream>
#include "AlgorithmRegistrar.h"
#include "GameManagerRegistrar.h"
#include "../UserCommon/OurSattelliteView.h"
#include "../UserCommon/MatrixObject.h"
#include "../UserCommon/UnmovingObject.h"
#include "../UserCommon/ObjectType.h"
#include "../UserCommon/MovingObject.h"
#include "../UserCommon/Orientation.h"
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
        };

        std::string fileToPrintPath;
        vector<shared_ptr<MapData>> mapsData;
        vector<string> algos;
        vector<string> gameManagers;
        vector<void*> handles;
        vector<runObj> runObjects;
        vector<pair<std::future<GameResult>, string>> results;
        vector<pair<GameResult, vector<string>>> comparativeGrouped;
        vector<pair<string, size_t>> competitionGrouped;

        void loadConfigFromInput(int argc, char const *argv[]);
        InputError validateInput(int argc, char const *argv[]);
        bool checkConfig();
        InputError getNamesComaprative();
        InputError getNamesCompetition();
        InputError loadMapsData();
        void loadAlgorithms();
        void loadGameManagers();
        void loadRunObjectsComparative();
        void loadRunObjectsCompetition();
        void sendRunObjectsToThreadPool(shared_ptr<ThreadPool> threadPool);
        void runRegularRunObjects();
        void sortResultsComparative();
        void printResultsComparative();
        void sortResultsCompetition();
        void printResultsCompetition();
        static Simulator simulator;
        vector<vector<array<shared_ptr<matrixObject>, 3>>> createSatView(const std::string& filePath, int numOfCols, int numOfRows);
        string parseGameResultReason(const GameResult& gr, int mapIndex) const;
        bool loadAndCheckAll(int argc, char const *argv[]);
        bool unloadAll();

        void comparativeRun();
        void competitionRun();

    public:
        static Simulator& getSimulator();
        void simulate(int argc, char const *argv[]);
        Simulator() = default;
        ~Simulator();
    };

}
