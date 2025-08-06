#pragma once

#include "AlgorithmRegistrar.h"
#include "GameManagerRegistrar.h"
#include "../UserCommon/OurSattelliteView.h"

using namespace UserCommon_0000;

using std::string, std::vector;
namespace Simulator_0000
{
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
            size_t mapWidth;
            size_t mapHeight;
            OurSattelliteView map;
            string mapName;
            size_t numShells;
            size_t maxTurns;
        };

        vector<MapData> mapsData;

        void loadConfigFromInput(int argc, char const *argv[]);
        bool validateInput(int argc, char const *argv[]);
        bool checkConfig();
        static Simulator simulator;
    public:
        static Simulator& getSimulator();
        void simulate(int argc, char const *argv[]);
        Simulator();
        ~Simulator();
    };
    

}
