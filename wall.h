#ifndef WALL_H
#define WALL_H
#include "unmovingObject.h"
class wall : public unmovingObject
{

public:
    wall(int x, int y, objectType oType);
    ~wall() override;
};

#endif // WALL_H