# Detect OS
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname -s)
endif

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Platform-specific configurations
ifeq ($(DETECTED_OS),Windows)
    # Windows settings
    CXXFLAGS += -DSFML_STATIC
    SFML_LIBS = -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lsfml-audio-s \
                -lopengl32 -lfreetype -lwinmm -lgdi32 \
                -lopenal32 -lflac -lvorbisenc -lvorbisfile -lvorbis -logg -lws2_32
    EXE_EXT = .exe
    RM = del /Q
else
    # Linux/macOS settings
    SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
    EXE_EXT =
    RM = rm -f
endif

# SFML paths (relative to project directory)
SFML_INCLUDE = -I./SFML/include
SFML_LIB = -L./SFML/lib

# Filesystem library
FS_LIB = -lstdc++fs

# Source files
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = main$(EXE_EXT)

# Default target
all: $(TARGET)

# Build rule
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SFML_LIB) $(SFML_LIBS) $(FS_LIB)

# Compile rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SFML_INCLUDE) -c $< -o $@

# Clean build files 
clean:
	$(RM) $(TARGET) $(OBJS)

# Run the application
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run