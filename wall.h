#ifndef WALL_H
#define WALL_H
#include "unmovingObject.h";
class wall : unmovingObject
{
    int hitsLeft;

public:
    wall(int x, int y);
    ~wall();
};

#endif // WALL_H