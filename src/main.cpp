//g++ -c -Wall .\src\main.cpp .\src\Particle.cpp .\src\ParticleSimulation.cpp .\src\QuadTree.cpp -I"C:\SFML-2.5.1\include" -I"C:\SFML-2.5.1\bin" -I".\include\"
//g++ .\main.o .\Particle.o .\ParticleSimulation.o .\Quadtree.o -o main -L"C:\SFML-2.5.1\lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

#include "ParticleSimulation.hpp"

// Delta Time
const float TIME_STEP = 0.002f;

// Window dimensions
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

int main()
{
    // Create the window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Particle Simulator");
    window.setFramerateLimit(60); // Limit the frame rate to 60 FPS

    //Gravity for particles
    sf::Vector2f grav = sf::Vector2f(0,0);

    //Start Particle Simulation
    ParticleSimulation particleSimulation(TIME_STEP, grav, window, 12);
    particleSimulation.run();

    return 0;
}
