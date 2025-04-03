#include "location.h";

class bullet{
    location bulletLocation;
    const int orientation;

    public:
    bullet(int x, int y, int orientation) : bulletLocation(x,y), orientation(orientation){
        // constructor
    }
};
