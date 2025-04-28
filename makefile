CPP = g++
CPP_FLAGS = -std=c++20 -g -Wall -Wextra -Werror -pedantic -fsanitize=address

# Target executable
TARGET = tanks_game

# Source files
SOURCES = tanksGame.cpp gameManager.cpp matrixObject.cpp tank.cpp bullet.cpp mine.cpp wall.cpp player1Tank.cpp player2Tank.cpp movingObject.cpp unmovingObject.cpp
HEADERS = gameManager.h matrixObject.h tank.h bullet.h mine.h move.h objType.h orientation.h wall.h playerTank.h movingObject.h unmovingObject.h tanksGame.h

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	$(CPP) $(CPP_FLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files into object files
tanksGame.o: tanksGame.cpp tanksGame.h gameManager.h
	$(CPP) $(CPP_FLAGS) -c tanksGame.cpp -o tanksGame.o

gameManager.o: gameManager.cpp gameManager.h matrixObject.h movingObject.h unmovingObject.h bullet.h move.h tank.h mine.h wall.h orientation.h playerTank.h
	$(CPP) $(CPP_FLAGS) -c gameManager.cpp -o gameManager.o

matrixObject.o: matrixObject.cpp matrixObject.h objectType.h
	$(CPP) $(CPP_FLAGS) -c matrixObject.cpp -o matrixObject.o

movingObject.o: movingObject.cpp movingObject.h matrixObject.h orientation.h
	$(CPP) $(CPP_FLAGS) -c movingObject.cpp -o movingObject.o

unmovingObject.o: unmovingObject.cpp unmovingObject.h matrixObject.h
	$(CPP) $(CPP_FLAGS) -c unmovingObject.cpp -o unmovingObject.o

bullet.o: bullet.cpp bullet.h movingObject.h orientation.h
	$(CPP) $(CPP_FLAGS) -c bullet.cpp -o bullet.o

mine.o: mine.cpp mine.h unmovingObject.h
	$(CPP) $(CPP_FLAGS) -c mine.cpp -o mine.o

wall.o: wall.cpp wall.h unmovingObject.h
	$(CPP) $(CPP_FLAGS) -c wall.cpp -o wall.o

tank.o: tank.cpp tank.h movingObject.h move.h bullet.h
	$(CPP) $(CPP_FLAGS) -c tank.cpp -o tank.o

player1Tank.o: player1Tank.cpp playerTank.h tank.h move.h
	$(CPP) $(CPP_FLAGS) -c player1Tank.cpp -o player1Tank.o

player2Tank.o: player2Tank.cpp playerTank.h tank.h move.h
	$(CPP) $(CPP_FLAGS) -c player2Tank.cpp -o player2Tank.o

# Clean up build files
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -rf log_file.txt
	rm -rf input_errors.txt

# Phony targets
.PHONY: all clean