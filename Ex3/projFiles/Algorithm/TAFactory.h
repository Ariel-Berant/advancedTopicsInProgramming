#ifndef TAFACTORY_H
#define TAFACTORY_H

#include "../common/TankAlgorithmFactory.h"
#include "PlayerTank.h"
#include "../UserCommon/Orientation.h"
#include <iostream>

class TAFactory : public TankAlgorithmFactory{
private:
    /* data */
public:
    TAFactory();
    ~TAFactory() override;

    std::unique_ptr<TankAlgorithm> create(
            int player_index, [[maybe_unused]] int tank_index) const override;
    
};

#endif// TAFactory.cpp