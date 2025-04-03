#include "location.h";
class wall{
    location loc;
    int hitsLeft;
    public:
    wall(int x, int y) : loc(x,y), hitsLeft(2){
        
    }
};
