#include "Simulator.h"

using namespace Simulator_0000;

Simulator Simulator::simulator;

Simulator& Simulator::getSimulator() {
    return simulator;
}

/* 
Function created with the help of Gemini: 
https://g.co/gemini/share/a85ba05001bc
*/
std::pair<size_t, size_t> parse_size_t_pair(const std::string& input_str) {
    // Create a stringstream from the input string to easily parse it.
    std::stringstream ss(input_str);

    size_t first, second;

    // Try to extract the two numbers. The '>>' operator automatically
    // skips whitespace between the numbers.
    ss >> first >> second;

    // Check for parsing errors. The stream's "fail" bit is set if the
    // extractions failed (e.g., if the string contained non-numeric characters).
    if (ss.fail()) {
        throw std::invalid_argument("Invalid format: could not parse two numbers.");
    }

    // Check for any leftover characters (other than whitespace).
    // If we can still extract something, it means the string has extra data.
    std::string remaining_content;
    ss >> remaining_content;
    if (!remaining_content.empty()) {
        throw std::invalid_argument("Invalid format: extra characters found after numbers.");
    }

    // Return the successfully parsed numbers as a pair.
    return {first, second};
}

string getTimeString() {
    using namespace std::chrono;
    auto now = system_clock::now();
    duration<double> ts = now.time_since_epoch();
    constexpr size_t NUM_DIGITS = 9;
    size_t NUM_DIGITS_P = std::pow(10, NUM_DIGITS);    
    std::ostringstream oss;
    oss << std::setw(NUM_DIGITS) << std::setfill('0') << size_t(ts.count() * NUM_DIGITS_P) % NUM_DIGITS_P;
    std::string s = oss.str();
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
    size_t currX = 0, currY = 0;

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

    if (config.runType == 1) // competition
    {
        if ((config.mapsFolderPath.empty() || config.gameManagerPath.empty() || config.algorithmsFolderPath.empty())
            || (!config.algorithm2Path.empty() || !config.algorithm1Path.empty() || !config.mapPath.empty() || !config.gameManagerFolderPath.empty())) 
        {
            return false;
        }
    }
    else if (config.runType == 2) // comparative
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


InputError Simulator::validateInput(int argc, char const *argv[])
{
    if (argc < 5 || argc > 8)
    {
        return InputError("Invalid number of arguments.");
    }

    loadConfigFromInput(argc, argv);

    if (!checkConfig())
    {
        return InputError("Invalid configuration.");
    }

    return InputError();
}

InputError checkIfDirectoryValid(const std::string& path) {
    std::filesystem::path dirPath(path);
    if (!std::filesystem::exists(dirPath)) {
        std::ostringstream oss;
        oss << "Directory does not exist: " << path;
        return InputError(oss.str());
    }
    if (!std::filesystem::is_directory(dirPath)) {
        std::ostringstream oss;
        oss << "Path is not a directory: " << path;
        return InputError(oss.str());
    }
    return InputError();
}

InputError checkIfFileValid(const std::string& filePath) {
    std::filesystem::path path(filePath);
    if (!std::filesystem::exists(path)) {
        std::ostringstream oss;
        oss << "File does not exist: " << filePath;
        return InputError(oss.str());
    }
    if (std::filesystem::path(filePath).extension() != ".so") {
        std::ostringstream oss;
        oss << "Invalid file extension (must be .so): " << filePath;
        return InputError(oss.str());
    }
    return InputError();
}



InputError Simulator::getNamesComaprative() {
    InputError ie = checkIfDirectoryValid(config.mapsFolderPath);
    if (ie.errorOccured) 
    {
        std::cerr << ie.message << std::endl;
        return ie;
    }

    ie = checkIfFileValid(config.algorithm1Path);
    if (ie.errorOccured) {
        std::cerr << ie.message;
        return ie;
    }

    ie = checkIfFileValid(config.algorithm2Path);
    if (ie.errorOccured) {
        std::cerr << ie.message;
        return ie;
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
        return;
    }

    fileToPrintPath = std::filesystem::path(config.gameManagerFolderPath).parent_path() / ("comparative_results_" + getTimeString() + ".txt");
}

InputError Simulator::getNamesCompetition() {

    if (checkIfFileValid(config.gameManagerPath).errorOccured)
    {
        std::ostringstream oss;
        oss << "Invalid game manager file path: " << config.gameManagerPath;
        return InputError(oss.str());
    }

    gameManagers.push_back(config.gameManagerPath);

    if (checkIfDirectoryValid(config.algorithmsFolderPath).errorOccured)
    {
        std::ostringstream oss;
        oss << "Invalid algorithms folder path: " << config.algorithmsFolderPath;
        return InputError(oss.str());
    }

    for (const auto& entry : std::filesystem::directory_iterator(config.algorithmsFolderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            algos.push_back(entry.path().string());
        }
    }

    if (algos.empty()) {
        std::cerr << "No valid algorithm files found in: " << config.algorithmsFolderPath << std::endl;
        return;
    }

    fileToPrintPath = std::filesystem::path(config.algorithmsFolderPath).parent_path() / ("competition_" + getTimeString() + ".txt");
}

InputError Simulator::loadMapsData() {
    if (checkIfDirectoryValid(config.mapsFolderPath).errorOccured) 
    {
        std::ostringstream oss;
        oss << "Invalid maps folder path: " << config.mapsFolderPath;
        return InputError(oss.str());
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
        std::ostringstream oss;
        oss << "No valid map files found in: " << config.mapsFolderPath;
        return InputError(oss.str());
    }

    return InputError();
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
            std::cerr << "---------------------------------" << std::endl;
            std::cerr << "BadRegistrationException for: " << manager << std::endl;
            std::cerr << "Name as registered: " << e.name << std::endl;
            std::cerr << "Has Game Manager factory? " << std::boolalpha << e.hasGameManagerFactory << std::endl;
            std::cerr << "---------------------------------" << std::endl;
            registrar.removeLast();   
        }
    }
}


void Simulator::loadRunObjectsComparative(){
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
            .player2 = std::move(algoRegistrar.getAtIndex(1).createPlayer(2, 
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

// Competition, so gameManager is the first
void Simulator::loadRunObjectsCompetition(){
    auto& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    auto& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();

    for (size_t k = 0; k < mapsData.size(); k++)
    {
        for (size_t i = 0; i < gameManagerRegistrar.count(); i++)
        {
            int versus = (i + 1 + (k % (gameManagerRegistrar.count() - 1))) % gameManagerRegistrar.count();
            runObj runObject{
                .player1 = std::move(algoRegistrar.getAtIndex(i).createPlayer(1, 
                                                                            mapsData[k]->mapWidth, 
                                                                            mapsData[k]->mapHeight, 
                                                                            mapsData[k]->maxTurns,
                                                                            mapsData[k]->numShells)),
                .player2 = std::move(algoRegistrar.getAtIndex(versus).createPlayer(2, 
                                                                            mapsData[k]->mapWidth, 
                                                                            mapsData[k]->mapHeight, 
                                                                            mapsData[k]->maxTurns,
                                                                            mapsData[k]->numShells)),
                .tankFactory1 = algoRegistrar.getAtIndex(i).getTankAlgorithmFactory(),
                .tankFactory2 = algoRegistrar.getAtIndex(versus).getTankAlgorithmFactory(),
                .gameManager = std::move(gameManagerRegistrar.getAtIndex(0).createGameManager(config.verbose)),
                .mapData = mapsData[k],
                .gameManagerName = std::to_string(i) + " " + std::to_string(versus),
                .algo1Name = algoRegistrar.getAtIndex(i).name(),
                .algo2Name = algoRegistrar.getAtIndex(versus).name()
            };

            runObjects.push_back(std::move(runObject));
        }
    }
}

void Simulator::sendRunObjectsToThreadPool(shared_ptr<ThreadPool> threadPool) {
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

void Simulator::runRegularRunObjects() {
    for (auto& runObject : runObjects)
    {
        std::promise<GameResult> res; 
        std::future<GameResult> futureRes = res.get_future();
        res.set_value(
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
            ));
        
        results.emplace_back(pair<std::future<GameResult>, string>(std::move(futureRes), runObject.gameManagerName));
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

    // Sorts by descending order - greatest size(amount of algorithms with this result) first
    sort(comparativeGrouped.begin(), comparativeGrouped.end(), [](const auto& a, const auto& b) {
        return a.second.size() > b.second.size();
    });
}

void Simulator::sortResultsCompetition(){
    competitionGrouped.clear();
    pair<size_t, size_t> algInds;
    AlgorithmRegistrar& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    for (size_t i = 0; i < registrar.count(); i++)
    {
        competitionGrouped.emplace_back(registrar.getAtIndex(i).name(), 0);
    }
    
    for (size_t i = 0; i < results.size(); i++)
    {
        algInds = parse_size_t_pair(results[i].second);
        if (results[i].first.get().winner == 0) {
            competitionGrouped[algInds.first].second++;
            competitionGrouped[algInds.second].second++;
        }
        else if (results[i].first.get().winner == 1) {
            competitionGrouped[algInds.first].second += 3;
        }
        else if (results[i].first.get().winner == 2) {
            competitionGrouped[algInds.second].second += 3;
        }
    }

    // Sorts by descending order - greatest score first
    sort(competitionGrouped.begin(), competitionGrouped.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });
}

string vectorToString(const vector<string>& vec) {
    string result;
    for (const auto& str : vec) {
        result += str + ", ";
    }
    if (!result.empty()) {
        result.pop_back(); // Remove last space
        result.pop_back(); // Remove last comma
    }
    return result;
}

string Simulator::parseGameResultReason(const GameResult& gr, int mapIndex) const{
    if( gr.remaining_tanks[0] == 0 && gr.remaining_tanks[1] == 0){
        return "Tie, both players have zero tanks";
    }
    else if(gr.remaining_tanks[0] == 0 || gr.remaining_tanks[1] == 0){
        int winnerPlayerNum = gr.remaining_tanks[1] != 0 ? 2 : 1;
        int tanksLeftToWinner = gr.remaining_tanks[1] != 0 ? gr.remaining_tanks[1] : gr.remaining_tanks[0];
        return "Player " + std::to_string(winnerPlayerNum) + " won with " + std::to_string(tanksLeftToWinner) + " tanks still alive";

    }
    else if(gr.reason == GameResult::MAX_STEPS){
        return "Tie, reached max steps = " + std::to_string(gr.rounds) + 
            ", player 1 has " + std::to_string(gr.remaining_tanks[0]) + " tanks, player 2 has " + std::to_string(gr.remaining_tanks[1]) + " tanks";
    }
    else if(gr.reason == GameResult::ZERO_SHELLS){
        return "Tie, both players have zero shells for " + std::to_string(mapsData[mapIndex]->maxTurns) + " steps";
    }
}

string satelliteViewToString(const SatelliteView& satelliteView){
    std::ostringstream oss;
    size_t currX = 0, currY = 0;

    while(satelliteView.getObjectAt(currX, currY) != '&')
    {
        while(satelliteView.getObjectAt(currX, currY) != '&')
        {
            oss << satelliteView.getObjectAt(currX, currY);
            currX++;
        }

        oss << "\n";

        currY++;
        currX = 0;
    }

    return oss.str();
}

void Simulator::printResultsCompetition(){
    std::ofstream outFile(fileToPrintPath);
    if (outFile.is_open())
    {
        outFile << "game_map=" << config.mapsFolderPath << std::endl;
        outFile << "algorithm1=" << config.gameManagerPath << std::endl;
        outFile << "algorithm2=" << config.algorithm2Path << std::endl;
        outFile << std::endl;

        for (const auto& group : competitionGrouped)
        {
            outFile << group.first << " " << group.second << std::endl;
        }
    }
}

void Simulator::printResultsComparative(){
    std::ofstream outFile(fileToPrintPath);
    if (outFile.is_open())
    {
        outFile << "game_map=" << config.mapPath << std::endl;
        outFile << "algorithm1=" << config.algorithm1Path << std::endl;
        outFile << "algorithm2=" << config.algorithm2Path << std::endl;
        outFile << std::endl;

        for (const auto& group : comparativeGrouped)
        {
            outFile << vectorToString(group.second) << std::endl;
            outFile << parseGameResultReason(group.first, 0) << std::endl;
            outFile << group.first.rounds << std::endl;
            outFile << satelliteViewToString(*(group.first.gameState.get())) << std::endl;
            outFile << std::endl; // Add an empty line between groups
        }
    }
    
}

void Simulator::comparativeRun(){
    if (config.threadsNum > 1)
    {
        shared_ptr<ThreadPool> threadPool = std::make_shared<ThreadPool>(config.threadsNum);
        sendRunObjectsToThreadPool(threadPool);
    }
    else
    {
        runRegularRunObjects();
    }

    sortResultsComparative();
    printResultsComparative();
}

void Simulator::competitionRun(){
    if (config.threadsNum > 1)
    {
        shared_ptr<ThreadPool> threadPool = std::make_shared<ThreadPool>(config.threadsNum);
        sendRunObjectsToThreadPool(threadPool);
    }
    else
    {
        runRegularRunObjects();
    }

    sortResultsCompetition();
    printResultsCompetition();
}

bool Simulator::loadAndCheckAll(int argc, char const *argv[]) {
    InputError ie = validateInput(argc, argv);
    if (ie.errorOccured) {
        std::cerr << ie.message << std::endl;
        return false;
    }

    ie = loadMapsData();
    if (ie.errorOccured)
    {
        std::cerr << ie.message << std::endl;
        return false;
    }

    if (config.runType == 2) ie = getNamesComaprative();
    if (config.runType == 1) ie = getNamesCompetition();
    
    if (ie.errorOccured)
    {
        std::cerr << ie.message << std::endl;
        return false;
    }

    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    GameManagerRegistrar::getGameManagerRegistrar().clear();
    AlgorithmRegistrar& algoRegistrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    GameManagerRegistrar& gameManagerRegistrar = GameManagerRegistrar::getGameManagerRegistrar();

    loadAlgorithms();
    if (algoRegistrar.count() == 0)
    {
        std::cerr << "No algorithms loaded." << std::endl;
        return false;
    }

    loadGameManagers();
    if (gameManagerRegistrar.count() == 0)
    {
        std::cerr << "No game managers loaded." << std::endl;
        return false;
    }

    if (config.runType == 2) loadRunObjectsComparative();
    if (config.runType == 1) loadRunObjectsCompetition();
}

bool Simulator::unloadAll(){
    for (size_t i = 0; i < handles.size(); i++)
    {
        if(dlclose(handles[i]) != 0)
        {
            std::cerr << "Failed to close handle: " << handles[i] << std::endl;
            return false;
        }
    }

    return true;
}

void Simulator::simulate(int argc, char const *argv[]) {
    try
    {
        if (!loadAndCheckAll(argc, argv)) {
            std::cerr << "Failed to load and check all critical components." << std::endl;
            return;
        }

        if (config.runType == 1) {
            competitionRun();
        } else if (config.runType == 2) {
            comparativeRun();
        }

        if (!unloadAll())
        {
            std::cerr << "Failed to unload all components." << std::endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

int main(int argc, char const *argv[])
{
    Simulator simulator;
    simulator.simulate(argc, argv);
    return 0;
}
