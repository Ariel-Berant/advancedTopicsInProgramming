# Compiler and flags
CPP = g++
CPP_FLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic -I../
LIBS = -ldl -pthread

# Target executable
TARGET = simulator_0000

# Source files
SOURCES = $(wildcard *.cpp)

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	$(CPP) -o $(TARGET) $(OBJECTS) $(LIBS)

# Compile source files into object files
%.o: %.cpp
	$(CPP) $(CPP_FLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all clean