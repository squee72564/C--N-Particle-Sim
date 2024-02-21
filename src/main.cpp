#include "ParticleSimulation.hpp"
#include <iostream>

// Fixed Delta Time - we need to change this
const float TIME_STEP = 0.000095f;

// Window dimensions
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

int main(int argc, char* argv[])
{
    int num_threads = 1;
    int max_depth = 8;
    int node_cap = 64;
    int simulation_width = WINDOW_WIDTH;
    int simulation_height = WINDOW_HEIGHT;

    if (argc < 6) {
        std::cout << "Usage: " << argv[0] << " <num_threads> <tree_max_depth> <tree_node_capacity> <sim_width> <sim_height>\n";
        return 1;
    } else {
        num_threads = std::atoi(argv[1]);
        max_depth = std::atoi(argv[2]);
        node_cap = std::atoi(argv[3]);
        simulation_width = std::atoi(argv[4]);
        simulation_height = std::atoi(argv[5]);

        if (max_depth > 10) max_depth = 10;

        if (!num_threads || !max_depth || !node_cap || !simulation_width || !simulation_height) {
            std::cout << "Usage: " << argv[0] << " <num_threads> <tree_max_depth> <tree_node_capacity> <sim_width> <sim_height>\n";
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
                                          max_depth,
                                          node_cap);

    std::cout << "Starting particle sim...\n";
    particleSimulation.run();
    std::cout << "Particle sim ended\n";

    return 0;
}
