#include "ParticleSimulation.hpp"
#include <iostream>

// Delta Time
const float TIME_STEP = 0.000095f;
//
//Gravity for particles
const sf::Vector2f grav = sf::Vector2f(0,0);

// Window dimensions
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

int main(int argc, char* argv[])
{
    int num_threads = 1;
    int max_depth = 8;
    int cap = 64;
    int simulation_width = WINDOW_WIDTH;
    int simulation_height = WINDOW_HEIGHT;

    if (argc < 6) {
        std::cout << "Usage: " << argv[0] << " <num threads> <tree max depth> <tree node capacity> <sim width> <sim height>\n";
        return 1;
    } else {
        num_threads = std::atoi(argv[1]);
        max_depth = std::atoi(argv[2]);
        cap = std::atoi(argv[3]);
        simulation_width = std::atoi(argv[4]);
        simulation_height = std::atoi(argv[5]);

        if (max_depth > 10) max_depth = 10;


        if (!num_threads || !max_depth || !cap || !simulation_width || !simulation_height) {
        std::cout << "Usage: " << argv[0] << " <num threads> <tree max depth> <tree node capacity> <sim width> <sim height>\n";
            std::cout << "--  Please ensure valid integers are passed as arguments.\n";
            return 1;
        }
    }

    // Create the window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Particle Simulator");
    window.setFramerateLimit(60); // Limit the frame rate to 60 FPS

    //Start Particle Simulation 
    ParticleSimulation particleSimulation(simulation_width,
                                          simulation_height,
                                          window,
                                          num_threads,
                                          TIME_STEP,
                                          grav,
                                          max_depth,
                                          cap);

    std::cout << "Starting particle sim...\n";
    particleSimulation.run();
    std::cout << "Particle sim ended\n";

    return 0;
}
