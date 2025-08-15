# Compiler and flags
CPP = g++
CPP_FLAGS = -std=c++20 -Wall -Wextra -fPIC -Werror -pedantic -g -I../
LIBS = -ldl -pthread -rdynamic

# Target executable
TARGET = simulator_0000

# Source files
SIMULATOR_SOURCES = $(wildcard *.cpp)
USER_COMMON_SOURCES = $(wildcard ../UserCommon/*.cpp)
SOURCES = $(SIMULATOR_SOURCES) $(USER_COMMON_SOURCES)

# Header files for dependency tracking
HEADERS = $(wildcard *.h) $(wildcard ../UserCommon/*.h) $(wildcard ../common/*.h)

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	$(CPP) -o $(TARGET) $(OBJECTS) $(LIBS)

# Compile source files into object files, now with header dependencies
%.o: %.cpp $(HEADERS)
	$(CPP) $(CPP_FLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all clean