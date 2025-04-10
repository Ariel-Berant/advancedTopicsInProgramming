#ifndef MOVE_H
#define MOVE_H

enum class move
{
    moveForward = 1,
    moveBackwords = 2,
    rotateEighthLeft = 3,
    rotateEighthRight = -4,
    rotatequarterLeft = 5,
    rotatequarterRight = -6,
    shoot = 7,
};
// This enum class defines the possible move for the tank object.
// It includes options for moving forward, backward, rotating left or right, and shooting.

#endif // MOVE_H