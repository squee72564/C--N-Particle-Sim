#include "ParticleSimulation.hpp"

// Delta Time
const float TIME_STEP = 0.002f;

// Window dimensions
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

int main()
{
    //Gravity for particles
    sf::Vector2f grav = sf::Vector2f(0,0);

    // Create the window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Particle Simulator");
    window.setFramerateLimit(60); // Limit the frame rate to 60 FPS

    //Start Particle Simulation
    ParticleSimulation particleSimulation(TIME_STEP, grav, window, 12, 6, 24);
    particleSimulation.run();

    return 0;
}
