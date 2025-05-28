#ifndef BULLET_H
#define BULLET_H
#include "MovingObject.h"

class bullet : public movingObject
{
public:
    bullet(int row, int col, orientation orient, objectType oType);
    ~bullet() override;
};

#endif // BULLET_H