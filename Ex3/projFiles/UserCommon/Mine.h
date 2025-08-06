#pragma once

#include "UnmovingObject.h"

namespace UserCommon_0000 {
    class mine : public unmovingObject
    {

    public:
        mine(int x, int y, objectType oType);
        ~mine() override;
    };

}
