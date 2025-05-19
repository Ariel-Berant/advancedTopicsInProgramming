#ifndef PFACTORY_H
#define PFACTORY_H

#include "../common/PlayerFactory.h"
#include "Player1.h"
#include "Player2.h"

class PFactory : PlayerFactory{
private:
    /* data */
public:
    PFactory(/* args */);
    ~PFactory();
    
    unique_ptr<Player> create(int player_index, size_t x, size_t y,
                                      size_t max_steps, size_t num_shells ) const override;
};

unique_ptr<Player> PFactory::create(int player_index, size_t x, size_t y,
                                      size_t max_steps, size_t num_shells ) const {
    return make_unique<Player1>(player_index, x, y, max_steps, num_shells);
}

#endif// PFactory.cpp