#ifndef BULLET_H
#define BULLET_H
#include "movingObject.h"

class bullet : public movingObject
{
public:
    bullet(int x, int y, orientation orient, objectType oType);
    ~bullet() override;
};

#endif // BULLET_H