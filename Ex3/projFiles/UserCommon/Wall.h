#pragma once

#include "UnmovingObject.h"

namespace UserCommon_0000 {

    class wall : public unmovingObject
    {

    public:
        wall(int row, int col, objectType oType);
        ~wall() override;
    };

}
