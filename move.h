#ifndef MOVE_H
#define MOVE_H

enum move
{
    moveForward = 1,
    moveBackwards = 2,
    rotateEighthLeft = 3,
    rotateEighthRight = 4,
    rotateQuarterLeft = 5,
    rotateQuarterRight = 6,
    shoot = 7,
    noAction = 8
};
// This enum class defines the possible move for the tank object.
// It includes options for moving forward, backward, rotating left or right, and shooting.

typedef enum move move;
#endif // MOVE_H