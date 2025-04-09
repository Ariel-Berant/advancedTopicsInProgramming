#ifndef MINE_h
#define MINE_h
#include "unmovingObject.h";
class mine : unmovingObject
{
    int hitsLeft;

public:
    mine(int x, int y);
};

#endif // MINE_h