# Compiler and flags
CPP = g++
CPP_FLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic -fPIC -g -I../
LDFLAGS = -shared

# Target shared library
TARGET = Algorithm_0000.so

# Source files
ALGORITHM_SOURCES = $(wildcard *.cpp)
USER_COMMON_SOURCES = $(wildcard ../UserCommon/*.cpp)
SOURCES = $(ALGORITHM_SOURCES) $(USER_COMMON_SOURCES)

# Header files for dependency tracking
HEADERS = $(wildcard *.h) $(wildcard ../UserCommon/*.h) $(wildcard ../common/*.h)

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files to create the shared library
$(TARGET): $(OBJECTS)
	$(CPP) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files into object files, now with header dependencies
%.o: %.cpp $(HEADERS)
	$(CPP) $(CPP_FLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all clean