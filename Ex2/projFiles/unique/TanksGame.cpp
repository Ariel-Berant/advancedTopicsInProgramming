#include "TanksGame.h"

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <map_file_path>" << endl;
        return 1;
    }
    string mapFilePath = argv[1];
    try
    {
        unique_ptr<TankAlgorithmFactory> ourTAFactory = make_unique<TAFactory>();
        unique_ptr<PlayerFactory> ourPlayerFactory = make_unique<PFactory>();
        gameManager gm(*ourTAFactory, *ourPlayerFactory);
        gm.readBoard(mapFilePath);
        gm.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Unexpected Error: " << e.what() << '\n';
        return(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}