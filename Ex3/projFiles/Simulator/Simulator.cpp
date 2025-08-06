#include "Simulator.h"

using namespace Simulator_0000;

Simulator Simulator::simulator;

Simulator& Simulator::getSimulator() {
    return simulator;
}

void Simulator::loadConfigFromInput(int argc, const char *argv[]){
    for (size_t i = 1; i < argc; i++)
    {
        string arg = argv[i];
        if (arg == "-comparative" || arg == "-competition")
        {
            if (arg == "-comparative")
            {
                config.runType = 2;
            }
            else
            {
                config.runType = 1;
            }
        }
        else if (arg == "-verbose")
        {
            config.verbose = true;
        }
        else if (arg.rfind("game_map=", 0) == 0)
        {
            config.mapPath = arg.substr(9);
        }
        else if (arg.rfind("game_maps_folder=", 0) == 0)
        {
            config.mapsFolderPath = arg.substr(17);
        }
        else if (arg.rfind("game_manager=", 0) == 0)
        {
            config.gameManagerPath = arg.substr(13);
        }
        else if (arg.rfind("game_managers_folder=", 0) == 0)
        {
            config.gameManagerFolderPath = arg.substr(21);
        }
        else if (arg.rfind("algorithm1=", 0) == 0)
        {
            config.algorithm1Path = arg.substr(11);
        }
        else if (arg.rfind("algorithm2=", 0) == 0)
        {
            config.algorithm2Path = arg.substr(11);
        }
        else if (arg.rfind("algorithms_folder=", 0) == 0)
        {
            config.algorithmsFolderPath = arg.substr(18);
        }
        else if (arg.rfind("num_threads=", 0) == 0)
        {
            config.threadsNum = std::stoi(arg.substr(12));
        }
        else{
            config.hasUnsupportedArgs = true;
        }
    }
}

bool Simulator::checkConfig() {
    if (config.hasUnsupportedArgs)
    {
        return false;
    }

    if (config.runType == 1)
    {
        // TODO: Ensure logic is correct for input validation
        if ((config.mapsFolderPath.empty() || config.gameManagerPath.empty() || config.algorithmsFolderPath.empty())
            || (!config.algorithm2Path.empty() || !config.algorithm1Path.empty() || !config.mapPath.empty() || !config.gameManagerFolderPath.empty())) 
        {
            return false;
        }
    }
    else if (config.runType == 2)
    {
        if ((!config.mapsFolderPath.empty() || !config.gameManagerPath.empty() || !config.algorithmsFolderPath.empty())
            || (config.algorithm2Path.empty() || config.algorithm1Path.empty() || config.mapPath.empty() || config.gameManagerFolderPath.empty()))
        {
            return false;
        }
    }
    else if (config.runType == 0)
    {
        std::cerr << "Run type not specified. Use -comparative or -competition." << std::endl;
        return false;
    }

    return true;
}


bool Simulator::validateInput(int argc, char const *argv[])
{
    if (argc < 5 || argc > 8)
    {
        std::cerr << "Invalid number of arguments." << std::endl;
        return false;
    }

    loadConfigFromInput(argc, argv);

    if (!checkConfig())
    {
        std::cerr << "Invalid configuration." << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char const *argv[])
{
        
    return 0;
}
