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
    //Gravity for particles
    sf::Vector2f grav = sf::Vector2f(0,0);

    // Get the current time
    long long timeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // Create the log file name with the current time
    std::string logName = "./log/performance-profile-" + std::to_string(timeInSeconds) + ".txt";

    std::ofstream outputFile(logName);

    outputFile << "NumThreads,TimeStep,QuadtreeMaxDepth,QuadTreeNodeCap,Iterations,AvgTimePerIter,PercentInserting,PercentleafNode,PercentUpdate,PercentMove,PercentDraw\n";

    outputFile.close();

    for (int i = 1; i <= 20; i++) { // 1 - 20 threads
        for (int j = 2; j <= 7; j++) { // 2 - 7 tree depth
            for (int k = 2; k <= 5; k++) { //2 - 5 node cap
                // Create the window
                sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Particle Simulator");
                window.setFramerateLimit(60); // Limit the frame rate to 60 FPS

                //Start Particle Simulation
                ParticleSimulation particleSimulation(TIME_STEP, grav, window, i, logName, k, j);
                particleSimulation.run();
            }
        }
    }

    return 0;
}
