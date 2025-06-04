# Compiler and flags
CPP = g++
CPP_FLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic

# Target executable
TARGET = tanks_game

# Source and header files (auto-detect all in unique/ and common/)
SRC_DIRS = unique common
SOURCES = $(wildcard $(addsuffix /*.cpp,$(SRC_DIRS)))
HEADERS = $(wildcard $(addsuffix /*.h,$(SRC_DIRS)))
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	$(CPP) $(CPP_FLAGS) -o $(TARGET) $(OBJECTS)

# Pattern rule for compiling .cpp to .o
%.o: %.cpp $(HEADERS)
	$(CPP) $(CPP_FLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -rf input_errors.txt

# Phony targets
.PHONY: all clean