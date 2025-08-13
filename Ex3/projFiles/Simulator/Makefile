# Compiler and flags
CPP = g++
CPP_FLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic -I../
LIBS = -ldl -pthread

# Target executable
TARGET = simulator_0000

# Source files
# Include sources from the current directory AND the UserCommon directory
SIMULATOR_SOURCES = $(wildcard *.cpp)
USER_COMMON_SOURCES = $(wildcard ../UserCommon/*.cpp)
SOURCES = $(SIMULATOR_SOURCES) $(USER_COMMON_SOURCES)

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