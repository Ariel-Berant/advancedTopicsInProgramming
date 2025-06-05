#ifndef PFACTORY_H
#define PFACTORY_H

#include "../common/PlayerFactory.h"
#include "OurPlayer.h"

class PFactory : public PlayerFactory{
private:
    /* data */
public:
    PFactory(/* args */);
    ~PFactory();
    
    std::unique_ptr<Player> create(int player_index, size_t x, size_t y,
                                      size_t max_steps, size_t num_shells ) const override;
};

#endif// PFactory.cpp