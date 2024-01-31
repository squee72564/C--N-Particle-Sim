#ifndef PARTICLE_SIMULATION
#define PARTICLE_SIMULATION

#include "Simulation.hpp"
#include "Particle.hpp"
#include "QuadTree.hpp"
#include "Profiler.hpp"
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>

class ParticleSimulation : Simulation
{
private:
    // Simulation threads
    int numThreads;

    // Game Window
    int windowWidth;
    int windowHeight;

    // Delta Time
    float timeStep;

    QuadTree quadTree;

    std::vector<Particle> particles;
    std::vector<QuadTree::Node*> leafNodes;
    std::vector<std::thread> threads;

    sf::Vector2f gravity;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dis;

    sf::Vector2f globalCOM;

    bool isRightButtonPressed;
    sf::Vector2f current_mousePosF;

    bool isAiming;
    sf::Vector2f initial_mousePosF;
    sf::Vector2f final_mousePosF;

    bool showVelocity;
    bool showQuadTree;

    bool isPaused;
    
    float particleMass;

    sf::Font font;
    sf::Text particleCountText;
    sf::Text particleMassText;
    sf::Text velocityText;
    sf::Text isPausedText;

    sf::Event event;

    unsigned long long iterationCount;
    unsigned long long totalTime;
    unsigned long long insertionTime;
    unsigned long long leafnodeTime;
    unsigned long long updateTime;
    unsigned long long moveTime;
    unsigned long long drawTime;

    std::string logfileName;

public:
    ParticleSimulation(float dt, const sf::Vector2f& g, sf::RenderWindow &window, int numThreads, int treeDepth, int nodeCap, std::string logfile);
    ParticleSimulation(float dt, const sf::Vector2f& g, sf::RenderWindow &window, int numThreads, int treeDepth, int nodeCap);
    virtual ~ParticleSimulation();
    void run();
    void pollUserEvent();
    void updateAndDraw();
    inline void drawAimLine();
    inline void drawParticleVelocity(Particle& particle);
    inline void attractParticleToMousePos(Particle& particle);
    void updateForces();
    void addParticleDiagonal(int tiles, int numParticles);
    void addParticleDiagonal2(int tiles, int particleNum);
};

#endif
