#ifndef MINE_h
#define MINE_h
#include "UnmovingObject.h"
class mine : public unmovingObject
{

public:
    mine(int x, int y, objectType oType);
    ~mine() override;
};

#endif // MINE_h