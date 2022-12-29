# CPP-N-Particle-Simulator

This is a N-particle simulator being implemented in C++ and using SFML.
SFML is cross-platform software development library designed to provide a simple application programming interface to various multimedia components in computers.

[SFML Website](https://www.sfml-dev.org/)

This particle simulator lets you place various size/mass particles in the window space, and will simulate a Zero-G environment like space where the particles interact.

Project is structured into the different directories:
  1. `docs` has all documentation
  2. `font` are fonts used for text objects for the simulation
  3. `include` is all header files for the C++ code
  4. `src` is all of the .cpp files for the C++ code

Implemented so far:
  * Spawn Particles of various sizes and mass:
    - Click and drag `Left Click` to launch a particle. Click and release the same spot without dragging to start with 0 velocity.
    - `Z` key to increase particle size/mass by 1
    - `X` key to decrease particle size/mass by 1
  * `Right Click` to attrack particles to mouse position
  * `I` key to show visualizations for particle velocity.
  * `G` key to show visualization for the quad tree.
  * Particles are inserted into QuadTree and QuadTree is built accordingly
  * Basic gravitational attraction between particles
    - Done in O(n<sup>2</sup>) brute force method right now
  * Basic particle collision when particles touch
    - Checks O(n<sup>2</sup>) brute force method right now
    
I plant to impelment the following features:
  * 2-D Barnes–Hut simulation
    - QuadTree to track collisions
    - Particle collisions need only be considered in nearby Quadtree cell
    - Particles from nearby QuadTree cells need to attract individually, and particles in distant cells can be treated as a single large particle centered at the cell's center of mass
    - Works in O(nlogn) as opposed to O(n<sup>2</sup>) brute force method
  * Accurate values for mass to better mimic stellar bodies in space.

![QuadTree](https://github.com/squee72564/CPP-N-Particle-Simulator/blob/main/QuadTree.jpg?raw=true)
![QuadTree 2](https://github.com/squee72564/CPP-N-Particle-Simulator/blob/main/QuadTree.jpg?raw=true)
![Velocity Vectors](https://github.com/squee72564/CPP-N-Particle-Simulator/blob/main/VelocityVec.jpg?raw=true)