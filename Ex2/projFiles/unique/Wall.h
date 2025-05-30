#ifndef WALL_H
#define WALL_H
#include "UnmovingObject.h"
class wall : public unmovingObject
{

public:
    wall(int row, int col, objectType oType);
    ~wall() override;
};

#endif // WALL_H