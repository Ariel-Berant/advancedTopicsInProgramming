#include "OurPlayer.h"

using namespace UserCommon_0000;

namespace Algorithm_0000 {

class Player1 : public OurPlayer {
public:
    Player1(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
        : OurPlayer(player_index, x, y, max_steps, num_shells) {
    }

    ~Player1() = default; // Destructor implementation
};

class Player2 : public OurPlayer {
public:
    Player2(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells)
        : OurPlayer(player_index, x, y, max_steps, num_shells) {
    }

    ~Player2() = default; // Destructor implementation
};

}