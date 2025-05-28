#ifndef TAFACTORY_H
#define TAFACTORY_H

#include "../common/TankAlgorithmFactory.h"
#include "PlayerTank.h"
#include "Orientation.h"
#include <iostream>





class TAFactory : TankAlgorithmFactory{
private:
    /* data */
public:
    TAFactory(/* args */);
    ~TAFactory();

    unique_ptr<TankAlgorithm> create(
            int player_index, int tank_index);
    
};

std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) {
    if (player_index == 1) {
        return std::make_unique<TankAlgorithm>(nullptr, nullptr, L);
    }
    else{
        return std::make_unique<TankAlgorithm>(nullptr, nullptr, R);
    }
}

#endif// TAFactory.cpp