#ifndef MOVE_H
#define MOVE_H

enum move
{
    moveForward = 1,
    moveBackwards = 2,
    rotateQuarterLeft = 3,
    rotateEighthLeft = 4,
    noAction = 5,
    rotateEighthRight = 6,
    rotateQuarterRight = 7,
    shoot = 8
};
// This enum class defines the possible move for the tank object.
// It includes options for moving forward, backward, rotating left or right, and shooting.

typedef enum move objMove;
#endif // MOVE_H