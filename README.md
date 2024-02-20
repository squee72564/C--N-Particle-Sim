
# CPP-N-Particle-Simulation
This is a N-particle simulator implemented in C++ and using SFML.

![ Simulation Gif](https://github.com/squee72564/CPP-N-Particle-Simulator/blob/main/docs/sim.gif)

SFML is cross-platform software development library designed to provide a simple application programming interface to various multimedia components in computers.

[SFML Website](https://www.sfml-dev.org/)

This particle simulator lets you place particles in the simulation space, and will simulate a Zero-G environment like space where the particles attract and collide.

## Project Structure
  1. `./docs/` has pictures and some documentation for the repo
  2. `./font/` are fonts used for text objects for the simulation
  3. `./include/` is all header files for the C++ code
  4. `./src/` is all of the .cpp files for the C++ code
  5. `./CMakeLists.txt` is a CMake file for building the application

## Building and Running the Project:
Use the CMakeLists.txt in the root directory of this project to build the application, as it allows for platform independent building. It is a sfml cmake template that will pull in the required SFML files for your OS [(see more here)](https://github.com/SFML/cmake-sfml-project/tree/master).

1. Dependencies:
	* [Git](https://git-scm.com/downloads)
	* [Cmake](https://cmake.org/download/)
	
2. Use CMake from the command line to build the project:

	For a single-configuration generator (typically the case on Linux and macOS):

	```
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build
	```

	For a multi-configuration generator (typically the case on Windows):

	```
	cmake -S . -B build
	cmake --build build --config Release
	```
	* If you do not have nmake installed on Windows you will get this error:
		```
		-- Building for: NMake Makefiles
		CMake Error at CMakeLists.txt:3 (project):
		  Running

		   'nmake' '-?'

		  failed with:

		   The system cannot find the file specified


		CMake Error: CMAKE_CXX_COMPILER not set, after EnableLanguage
		-- Configuring incomplete, errors occurred!
		```
	* You can alternatively [choose another generator like MingGW Makesfiles](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) if available:
		```
		cmake -S . -B build -G "MinGW Makefiles"
		cmake --build build --config Release
		```
3. Navigate to the ./build/bin/ directory that was created.
4. Within ./build/bin/ will be `main.exe` which is the executable for the program. To start the program run the executable with the following command line arguments:
	*  `.\build\bin\main.exe <num threads> <tree max depth> <tree node capacity> <sim width> <sim height>`
	
	* i.e. : `.\build\bin\main.exe 8 8 64 2000 2000`
		* Will run the program with 8 threads, a max depth of 8 for the quadtree, 64 as the node capacity for the quadtree, and 2000 x 2000 dimension for the simulation space. 


I find the best performance with the following:
* Number of threads == actual cores for CPU
* Quad Tree depth is best around 8 but play with it on your own computer
* Node capacity depends on the max depth and size of the simulation; play around to find the right balance 

## Implemented so far:
  * Quad Tree structure to track particle positions
  * Particles with variable mass:
    - Click and drag `Left Click` to launch a particle. Click and release the same spot without dragging to start with 0 velocity.
  * `Z` key to decrease max quad tree depth by 1
  * `X` key to increase max quad tree depth by 1
  * `Right Click` to attract particles to mouse position
  * `1` key to show visualizations for particle velocity
  * `2` key to show visualization for the quad tree
  * `3` key to show visualization for the particles themselves
  
  The performance will depend on your computer specs, and the parameters given when executing the program for the number of threads, and Quad Tree depth/node capacity.

  TODO:
  * Determine better values for delta time, gravitational constant, and particle masses to create a smoother and more realistic simulation
  * Fix collisions so that particles cannot get stuck together
  * Better UI such as font and additional information for graphical elements toggled on/off