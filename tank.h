using namespace std;
#include "location.h";

class tank{
    location loc;
    float orientation;
    int shotsLeft;
    int inBackwords;
    const size_t tank_size;
    public:
    tank(int x, int y, float orientation, size_t tank_size) : loc(x,y), orientation(orientation), tank_size(tank_size), shotsLeft(16), inBackwords(0){

    }

};





