#ifndef TAFACTORY_H
#define TAFACTORY_H

#include "../common/TankAlgorithmFactory.h"
#include "PlayerTank.h"
#include "Orientation.h"
#include <iostream>

class TAFactory : public TankAlgorithmFactory{
private:
    /* data */
public:
    TAFactory(/* args */);
    ~TAFactory();

    std::unique_ptr<TankAlgorithm> create(
            int player_index, int tank_index) const override;
    
};

#endif// TAFactory.cpp