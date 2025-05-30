#ifndef ORIENTATION_H
#define ORIENTATION_H

enum orientation {
    U = 0, // Up
    UR = 1, // Up-Right
    R = 2, // Right
    DR = 3, // Down-Right
    D = 4, // Down
    DL = 5, // Down-Left
    L = 6, // Left
    UL = 7, // Up-Left
    UNKNOWN =8 // Unknown
};

typedef enum orientation orientation;
#endif //ORIENTATION_H

