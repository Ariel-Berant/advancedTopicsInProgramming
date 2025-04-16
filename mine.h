#ifndef MINE_h
#define MINE_h
#include "unmovingObject.h"
#include "matrixObject.h"
class mine : public unmovingObject
{

public:
    mine(int x, int y, objectType oType);
    ~mine();
};

#endif // MINE_h