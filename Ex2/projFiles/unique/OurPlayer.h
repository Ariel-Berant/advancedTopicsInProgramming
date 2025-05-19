#ifndef OURPLAYER_H
#define OURPLAYER_H

#include "../common/Player.h"

class OurPlayer : public Player
{
protected:
        int player_index;
        size_t x;
        size_t y;
        size_t max_steps;
        size_t num_shells;
public:
    OurPlayer(int player_index,
            size_t x, size_t y,
            size_t max_steps, size_t num_shells);
    ~OurPlayer();   
};

#endif// OurPlayer.h