//
// Created by ariel on 4/23/2025.
//

#include "tanksGame.h"

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
        gameManager gm(mapFilePath);
        gm.playGame();
        gm.~gameManager();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Unexpected Error: " << e.what() << '\n';
        return(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}