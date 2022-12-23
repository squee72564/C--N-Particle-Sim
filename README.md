# CPP-N-Particle-Simulator

This is a N-particle simulator being implemented in C++ and using SFML.
SFML is cross-platform software development library designed to provide a simple application programming interface to various multimedia components in computers.

[SFML Website](https://www.sfml-dev.org/)

This particle simulator lets you place various size/mass particles in the window space, and will simulate a 0G environment like space where the particles interact.

Implemented so far:
  * Spawn Particles of various sizes and mass:
    - `Left Click` to spawn a particle
    - `Z` key to increase particle size/mass by 1
    - `X` key to decrease particle size/mass by 1
  * `Right Click` to attrack particles to mouse position
  * Basic gravitational attraction between particles
    - Done in O(n<sup>2</sup>) brute force method
  * Basic particle collision when particles touch
    - Checks O(n<sup>2</sup>) brute force method


I plant to impelment the following features:
  * 2-D Barnes–Hut simulation
    - QuadTree to track collisions
    - Particle collisions need only be considered in nearby Quadtree cell
    - Particles from nearby QuadTree cells need to attract individually, and particles in distant cells can be treated as a single large particle centered at the cell's center of mass
    - Works in O(nlogn) as opposed to O(n<sup>2</sup>) brute force method
  * Ability to click and drag when creating particles to launch a particle with a starting velocity
  * Toggle to draw information on screen such as particles velocity vector, and Quadtree visualization.
  * Accurate values for mass to better mimic stellar bodies in space.
