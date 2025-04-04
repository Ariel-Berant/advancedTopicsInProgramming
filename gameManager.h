#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class gameManager
{
private:
    int turns;
    int noBulletsCnt;

public:
    gameManager(/* args */);
    ~gameManager();
};

gameManager::gameManager(/* args */)
{
}

gameManager::~gameManager()
{
}

#endif // GAME_MANAGER_H