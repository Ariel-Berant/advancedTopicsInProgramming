#ifndef WALL_H
#define WALL_H
#include "unmovingObject.h"
#include "matrixObject.h"
class wall : public unmovingObject
{

public:
    wall(int x, int y, objectType oType);
    ~wall();
};

#endif // WALL_H