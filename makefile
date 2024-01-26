# Compiler
CXX = g++

# Compilation flags
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -O3

# Source files
SOURCES = src/main.cpp src/Particle.cpp src/ParticleSimulation.cpp src/Quadtree.cpp

# Include directories
INCLUDES = -I"./SFML-2.5.1/include" -I"./include/"

# Library directories
LIB_DIRS = -L"./SFML-2.5.1/lib"

# Libraries to link
LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lpthread

# Output executable
OUTPUT = main

# Makefile targets
all: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDES) $(LIB_DIRS) $(LIBS)

# Clean target
clean:
	rm -f $(OUTPUT)
