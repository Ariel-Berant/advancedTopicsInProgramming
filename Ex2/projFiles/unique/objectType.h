#ifndef OBJECTTYPE_H
#define OBJECTTYPE_H
enum objectType {
    W = 0,//wall
    B = 1,//bullet
    M = 2,//mine
    P1T = 3,//tank of player 1
    P2T = 4,// tank of player 2
    Blank = 5// empty space
};

typedef enum objectType objectType;
#endif //OBJECTTYPE_H
