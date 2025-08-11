#include "Simulator.h"

using namespace Simulator_0000;

Simulator Simulator::simulator;

Simulator& Simulator::getSimulator() {
    return simulator;
}

bool checkIfRemainingTanksSame(const GameResult& gameResults1, const GameResult& gameResults2) {
    if (gameResults1.remaining_tanks.size() != gameResults2.remaining_tanks.size())
    {
        return false;
    }

    for (size_t i = 0; i < gameResults1.remaining_tanks.size(); ++i)
    {
        if (gameResults1.remaining_tanks[i] != gameResults2.remaining_tanks[i])
        {
            return false;
        }
    }

    return true;
}

bool checkIfSatelliteViewSame(const GameResult& gameResults1, const GameResult& gameResults2) {
    int currX = 0, currY = 0;

    while(gameResults1.gameState->getObjectAt(currX, currY) != '&')
    {
        while(gameResults1.gameState->getObjectAt(currX, currY) != '&')
        {
            if (gameResults1.gameState->getObjectAt(currX, currY) != gameResults2.gameState->getObjectAt(currX, currY))
            {
                return false;
            }
            currX++;
        }

        if (gameResults2.gameState->getObjectAt(currX, currY) != '&')
        {
            return false;
        }
        currY++;
        currX = 0;
    }

    if (gameResults2.gameState->getObjectAt(currX, currY) != '&')
    {
        return false;
    }

    return true;
}

bool checkIfGameResultsSame(const GameResult& gameResults1, const GameResult& gameResults2) {
    bool same = true;

    if (gameResults1.winner != gameResults2.winner) same = false;
    if (gameResults1.reason != gameResults2.reason) same = false;
    if (checkIfRemainingTanksSame(gameResults1, gameResults2)) same = false;
    if (gameResults1.rounds != gameResults2.rounds) same = false;
    if (!checkIfSatelliteViewSame(gameResults1, gameResults2)) same = false;

    return same;
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

bool checkIfDirectoryValid(const std::string& path) {
    std::filesystem::path dirPath(path);
    if (!std::filesystem::exists(dirPath)) {
        std::cerr << "Directory does not exist: " << path << std::endl;
        return false;
    }
    if (!std::filesystem::is_directory(dirPath)) {
        std::cerr << "Path is not a directory: " << path << std::endl;
        return false;
    }
    return true;
}

bool checkIfFileValid(const std::string& filePath) {
    std::filesystem::path path(filePath);
    if (!std::filesystem::exists(path)) {
        std::cerr << "File does not exist: " << filePath << std::endl;
        return false;
    }
    if (std::filesystem::path(filePath).extension() != ".so") {
        std::cerr << "Invalid file extension (must be .so): " << filePath << std::endl;
        return false;
    }
    return true;
}



void Simulator::getNamesComaprative() {
    if (!checkIfDirectoryValid(config.gameManagerFolderPath)) 
    {
        std::cerr << "Invalid game manager folder path: " << config.gameManagerFolderPath << std::endl;
        return;
    }
    
    if (!checkIfFileValid(config.algorithm1Path) || !checkIfFileValid(config.algorithm2Path))
    {
        std::cerr << "Invalid algorithm file paths." << std::endl;
        return;
    }
    
    algos.push_back(config.algorithm1Path);
    algos.push_back(config.algorithm2Path);

    for (const auto& entry : std::filesystem::directory_iterator(config.gameManagerFolderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            gameManagers.push_back(entry.path().string());
        }
    }

    if (gameManagers.empty()) {
        std::cerr << "No valid game manager files found in: " << config.gameManagerFolderPath << std::endl;
    }
}

void Simulator::getNamesCompetition() {

    if (!checkIfFileValid(config.gameManagerPath))
    {
        std::cerr << "Invalid game manager file path: " << config.gameManagerPath << std::endl;
        return;
    }

    gameManagers.push_back(config.gameManagerPath);
    
    if (!checkIfDirectoryValid(config.algorithmsFolderPath))
    {
        std::cerr << "Invalid algorithms folder path: " << config.algorithmsFolderPath << std::endl;
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(config.algorithmsFolderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            algos.push_back(entry.path().string());
        }
    }

    if (algos.empty()) {
        std::cerr << "No valid algorithm files found in: " << config.algorithmsFolderPath << std::endl;
    }
}

void Simulator::loadMapsData() {
    if (!checkIfDirectoryValid(config.mapsFolderPath)) 
    {
        std::cerr << "Invalid maps folder path: " << config.mapsFolderPath << std::endl;
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(config.mapsFolderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            pair<string, array<size_t, 4>> mapInfo = parseMapLine(entry.path().string());
            if (mapInfo.second[0] == 0 || mapInfo.second[1] == 0 || mapInfo.second[2] == 0 || mapInfo.second[3] == 0) {
                continue;
            }

            MapData mapData{
                mapInfo.first,
                mapInfo.second[0],
                mapInfo.second[1],
                mapInfo.second[2],
                mapInfo.second[3],
                (OurSattelliteView)
                {
                    createSatView(entry.path().string(), mapInfo.second[3], mapInfo.second[2]),
                    mapInfo.second[3],
                    mapInfo.second[2],
                    -100, // tankX, initialized to 0
                    -100  // tankY, initialized to 0
                }
            };
            mapsData.push_back(std::make_shared<MapData>(mapData));
        }
    }

    if (mapsData.empty()) {
        std::cerr << "No valid map files found in: " << config.mapsFolderPath << std::endl;
    }
}

pair<string, array<size_t, 4>> parseMapLine(const string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return {"", {}};
    }

    string mapName;
    array<size_t, 4> dimensions = {0, 0, 0, 0}; // {maxTurns, numShells, mapHeight, mapWidth}
    string line;
    size_t currValue = -1;

    for (size_t i = 0; i < 5; i++)
    {
        getline(file, line);
        if (i == 0) 
        {
            if (line.empty()) {
                std::cerr << "Error: The first line of the map file is empty." << std::endl;
                return {"", {}};
            }
            mapName = line;
        }
        else
        {
            if (i==1)
            {
                currValue = parseRowInfo(line, "MaxSteps=", 1);
            }
            else if (i==2) 
            {
                currValue = parseRowInfo(line, "NumShells=", 2);
            }
            else if (i==3) 
            {
                currValue = parseRowInfo(line, "Rows=", 3);
            }
            else if (i==4) 
            {
                currValue = parseRowInfo(line, "Cols=", 4);
            }

            if (currValue <= 0) {
                std::cerr << "Error: Invalid format or value in line " << i + 1 << " of the map file." << std::endl;
                return {"", {}};
            }
            dimensions[i - 1] = currValue; // Store the value in the corresponding index
        }
    }

    file.close();
    return {mapName, dimensions};
}


size_t parseRowInfo(const string line, const string description, int rowNum) {
    string fileRow = line;
    // Remove all whitespace characters from the string
    fileRow.erase(remove_if(fileRow.begin(), 
                              fileRow.end(),
                              [](unsigned char x) { return std::isspace(x); }),
               fileRow.end());
    if (fileRow.find(description) != 0) {
        std::cerr << "Error: The row for \"" << description << "\" does not start with the expected description.\n" << std::endl;
        return false;
    }
    fileRow.erase(0, description.length());
    // Check if the remaining string is a valid integer
    if (!all_of(fileRow.begin(), fileRow.end(), ::isdigit)) {
        std::cerr << "Error: The row for \"" << description << "\" contains invalid characters.\nCorrect use is: "
             << description << "<NUM>\n" << std::endl;
        return false;
    }
    size_t value = stoul(fileRow);
    if (rowNum == 1)
    {  
        return value * 2; // maxTurns
    }
    return value; // numShells, mapHeight, mapWidth
}

vector<vector<array<shared_ptr<matrixObject>, 3>>> Simulator::createSatView(const std::string& filePath, int numOfCols, int numOfRows) {
    vector<vector<array<shared_ptr<matrixObject>, 3>>> gameBoard;
    string line;
    size_t currRow = 0, currCol = 0;

    gameBoard.resize(numOfCols, vector<array<shared_ptr<matrixObject>, 3>>(numOfRows, {nullptr, nullptr, nullptr}));

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return gameBoard;
    }

    while (getline(file, line)){
        if (currRow == numOfRows){
        std::cerr << "Error: Too many rows in the map file.\n" << std::endl;
            break;
        }
        for (char ch : line) {
            if (currCol == numOfCols){
                std::cerr << "Error: Too many columns in row " << currRow << ".\n" << std::endl;
                continue;
            }
            switch (ch) {
            case '#':
                addUnmovingObjectToMap(gameBoard, '#', currCol, currRow);
                break;
            case '1':
                addTankToMap(gameBoard, 1, currCol, currRow);
                break;
            case '2':
                addTankToMap(gameBoard, 2, currCol, currRow);
                break;
            case '@':
                addUnmovingObjectToMap(gameBoard, '@', currCol, currRow);
                break;
            case ' ':
                break;
            default:
                std::cerr << "Error: unrecognized character, ASCII #'" << ch << "' in the map file.\n" << std::endl;
            }
            currCol++;
        }
        if (currCol < numOfCols){
            std::cerr << "Error: Not enough columns in row " << currRow << ".\n" << std::endl;
        }
        currRow++;
        currCol = 0;
    }
    if (currRow < numOfRows){
        std::cerr << "Error: Not enough rows in the map file.\n" << std::endl;
    }
    file.close();

    return gameBoard;
}

void addTankToMap(vector<vector<array<shared_ptr<matrixObject>, 3>>>& gameBoard, int playerNum, int currCol, int currRow) {
    if (playerNum == 1) {
        gameBoard[currCol][currRow][1] = make_unique<matrixObject>(currRow, currCol, P1T);
    } else if (playerNum == 2) {
        gameBoard[currCol][currRow][1] = make_unique<matrixObject>(currRow, currCol, P2T);
    }
}

void addUnmovingObjectToMap(vector<vector<array<shared_ptr<matrixObject>, 3>>>& gameBoard, char UnmovingObjectType, int currCol, int currRow){
    if(UnmovingObjectType == '#'){
        gameBoard[currCol][currRow][0] = make_unique<matrixObject>(currRow, currCol, W);
    }
    else if(UnmovingObjectType == '@'){
        gameBoard[currCol][currRow][0] = make_unique<matrixObject>(currRow, currCol, M);
    }
}

void Simulator::loadAlgorithms() {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    for(const auto& algo: algos) {
        registrar.createAlgorithmFactoryEntry(algo);

        void* handle = dlopen(algo.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if (!handle) {
            const char* error = dlerror();
            std::cerr << "Error loading algorithm: " << algo << std::endl;
            std::cerr << "dlerror: " << (error ? error : "Unknown error") << std::endl;
        }

        handles.push_back(handle);
        
        try {
            registrar.validateLastRegistration();
        }
        catch(AlgorithmRegistrar::BadRegistrationException& e) {
            // TODO: report according to requirements
            std::cerr << "---------------------------------" << std::endl;
            std::cerr << "BadRegistrationException for: " << algo << std::endl;
            std::cerr << "Name as registered: " << e.name << std::endl;
            std::cerr << "Has tank algorithm factory? " << std::boolalpha << e.hasTankAlgorithmFactory << std::endl;
            std::cerr << "Has Player factory? " << std::boolalpha << e.hasPlayerFactory << std::endl;
            std::cerr << "---------------------------------" << std::endl;
            registrar.removeLast();
        }
    }
}

void Simulator::loadGameManagers() {
    auto& registrar = GameManagerRegistrar::getGameManagerRegistrar();
    for(const auto& manager: gameManagers) {
        registrar.createGameManagerFactoryEntry(manager);

        void* handle = dlopen(manager.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if (!handle) {
            const char* error = dlerror();
            std::cerr << "Error loading game manager: " << manager << std::endl;
            std::cerr << "dlerror: " << (error ? error : "Unknown error") << std::endl;
        }

        handles.push_back(handle);

        try {
            registrar.validateLastRegistration();
        }
        catch(GameManagerRegistrar::BadRegistrationException& e) {
            // TODO: report according to requirements
            std::cerr << "---------------------------------" << std::endl;
            std::cerr << "BadRegistrationException for: " << manager << std::endl;
            std::cerr << "Name as registered: " << e.name << std::endl;
            std::cerr << "Has Game Manager factory? " << std::boolalpha << e.hasGameManagerFactory << std::endl;
            std::cerr << "---------------------------------" << std::endl;
            registrar.removeLast();   
        }
    }
}


// Comparative run, so we know that the first algo is loaded 
void Simulator::loadRunObjects(){
    auto& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();

    for (size_t i = 0; i < gameManagerRegistrar.count(); i++)
    {
        runObj runObject{
            .player1 = std::move(algoRegistrar.getAtIndex(0).createPlayer(1, 
                                                                        mapsData[0]->mapWidth, 
                                                                        mapsData[0]->mapHeight, 
                                                                        mapsData[0]->maxTurns,
                                                                         mapsData[0]->numShells)),
            .player2 = std::move(algoRegistrar.getAtIndex(0).createPlayer(2, 
                                                                        mapsData[0]->mapWidth, 
                                                                        mapsData[0]->mapHeight, 
                                                                        mapsData[0]->maxTurns,
                                                                         mapsData[0]->numShells)),
            .tankFactory1 = algoRegistrar.getAtIndex(0).getTankAlgorithmFactory(),
            .tankFactory2 = algoRegistrar.getAtIndex(1).getTankAlgorithmFactory(),
            .gameManager = std::move(gameManagerRegistrar.getAtIndex(i).createGameManager(config.verbose)),
            .mapData = mapsData[0],
            .gameManagerName = gameManagerRegistrar.getAtIndex(i).name(),
            .algo1Name = algoRegistrar.getAtIndex(0).name(),
            .algo2Name = algoRegistrar.getAtIndex(1).name()
        };

        runObjects.push_back(std::move(runObject));
    }
}

void Simulator::sendRunObjectsToThreadPool(unique_ptr<ThreadPool> threadPool) {
    for (auto& runObject : runObjects) {
        results.emplace_back(threadPool->enqueue([runObject]() mutable {
            runObject.gameManager->run(
                runObject.mapData->mapWidth,
                runObject.mapData->mapHeight,
                runObject.mapData->map,
                runObject.mapData->mapName,
                runObject.mapData->maxTurns,
                runObject.mapData->numShells,
                *(runObject.player1),
                runObject.algo1Name,
                *(runObject.player2),
                runObject.algo2Name,
                runObject.tankFactory1,
                runObject.tankFactory2
            );
        }), runObject.gameManagerName);
    }
}

void Simulator::sortResultsComparative(){
    for (size_t i = 0; i < results.size(); i++)
    {
        for (size_t j = 0; j < comparativeGrouped.size(); j++)
        {
            if (checkIfGameResultsSame(comparativeGrouped[j].first, results[i].first.get()))
            {
                comparativeGrouped[j].second.emplace_back(results[i].second);
                break;
            }
        }

        comparativeGrouped.emplace_back(results[i].first.get(), vector<string>{results[i].second});
    }

    sort(comparativeGrouped.begin(), comparativeGrouped.end(), [](const auto& a, const auto& b) {
        return a.second.size() < b.second.size();
    });
}

void Simulator::comparativeRun(){

}

int main(int argc, char const *argv[])
{       
    return 0;
}
