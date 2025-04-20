cmake_minimum_required(VERSION 3.29)
project(advancedTopicsInProgramming)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(advancedTopicsInProgramming
        bullet.cpp
        bullet.h
        gameManager.cpp
        gameManager.h
        matrixObject.cpp
        matrixObject.h
        mine.cpp
        mine.h
        move.h
        movingObject.cpp
        movingObject.h
        tank.cpp
        tank.h
        unmovingObject.cpp
        unmovingObject.h
        wall.cpp
        wall.h
        orientation.h
        gameManager.cpp
        gameManager.h
        gameManager.h
        player1Tank.cpp
        player2Tank.cpp
        playerTank.h)
