#ifndef TAFACTORY_H
#define TAFACTORY_H

#include "../common/TankAlgorithmFactory.h"
#include "PlayerTank.h"
#include "../UserCommon/Orientation.h"
#include <iostream>

using namespace UserCommon_0000;

namespace Algorithm_0000 {
    class TAFactory : public TankAlgorithmFactory{
    private:
        /* data */
    public:
        TAFactory();
        ~TAFactory() override;

        std::unique_ptr<TankAlgorithm> create(
                int player_index, [[maybe_unused]] int tank_index) const override;
        
    };
}
#endif // TAFactory.h