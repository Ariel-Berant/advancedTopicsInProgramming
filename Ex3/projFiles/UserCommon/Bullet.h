#pragma once

#include "MovingObject.h"
namespace UserCommon_0000{
    class bullet : public movingObject
    {
    public:
        bullet(int row, int col, orientation orient, objectType oType);
        ~bullet() override;
    };

}