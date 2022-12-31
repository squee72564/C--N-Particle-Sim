//g++ .\main.o .\Particle.o .\ParticleSimulation.o -o main -L"C:\SFML-2.5.1\lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
//g++ -c -Wall .\src\main.cpp .\src\Particle.cpp .\src\ParticleSimulation.cpp -I"C:\SFML-2.5.1\include" -I"C:\SFML-2.5.1\bin" -I".\include\"

#include "ParticleSimulation.hpp"

// Delta Time
const float TIME_STEP = 0.003f;

// Window dimensions
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 800;

int main()
{
    // Create the window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Particle Simulator");
    window.setFramerateLimit(60); // Limit the frame rate to 60 FPS

    //Gravity for particles
    sf::Vector2f grav = sf::Vector2f(0,0);

    //Start Particle Simulation
    ParticleSimulation particleSimulation(0.003f, grav, window);
    particleSimulation.run();

    return 0;
}
