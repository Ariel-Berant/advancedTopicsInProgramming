#ifndef MINE_h
#define MINE_h
#include "UnmovingObject.h"

namespace UserCommon_0000 {
    class mine : public unmovingObject
    {

    public:
        mine(int x, int y, objectType oType);
        ~mine() override;
    };

}

#endif // MINE_h