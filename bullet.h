#ifndef BULLET_H
#define BULLET_H
#include "movingObject.h";

class bullet : movingObject
{
public:
    bullet(int x, int y, int orientation);
};

#endif // BULLET_H