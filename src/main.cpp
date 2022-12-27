//g++ main.o Particle.o -o main -L"C:\SFML\lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
//g++ -c main.cpp Particle.cpp -I"C:/SFML/include" -I"C:/SFML/bin"

#include "ParticleSimulation.hpp"
#include <SFML/Graphics.hpp>

// Delta Time
const float TIME_STEP = 0.003f;

// Window dimensions
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 800;

//sf::Vector2f getMousePostion(const sf::RenderWindow &window, sf::Vector2i &mousePos);

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
