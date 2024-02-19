#ifndef PARTICLE_SIMULATION
#define PARTICLE_SIMULATION

#include "Particle.hpp"
#include "QuadTree.hpp"
#include "Profiler.hpp"
#include <vector>
#include <thread>

class ParticleSimulation
{
private:
    // Simulation threads
    sf::RenderWindow* gameWindow;
    int numThreads;
    int treeMaxDepth;

    // Game Window
    int simulationWidth;
    int simulationHeight;

    sf::View gameView;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dis;

    float timeStep;

    float particleMass;

    sf::Vector2f gravity;
    sf::Vector2f globalCOM;

    sf::Vector2f current_mousePosF;
    sf::Vector2f initial_mousePosF;
    sf::Vector2f final_mousePosF;
    sf::Vector2f scroll_mousePosF;

    bool isRightButtonPressed;
    bool isMiddleButtonPressed;
    bool isAiming;
    bool showVelocity;
    bool showQuadTree;
    bool showParticles;
    bool isPaused;

    sf::Font font;
    sf::Text particleCountText;
    sf::Text particleMassText;
    sf::Text velocityText;
    sf::Text isPausedText;

    sf::Event event;

    std::vector<std::thread> threads;
    std::vector<QuadTree::Node*> leafNodes;
    std::vector<Particle> particles;

    QuadTree quadTree;

public:
    ParticleSimulation(int simulationWidth,
                       int simulationHeight,
                       sf::RenderWindow &window,
                       int numThreads,
                       float dt,
                       const sf::Vector2f& g,
                       int treeDepth,
                       int nodeCap);

    ~ParticleSimulation();

    void run();
    void pollUserEvent();
    void updateAndDraw();

    inline void drawAimLine();
    inline void drawParticleVelocity();

    void updateForces();

    void addSierpinskiTriangleParticleChunk(int x, int y, int size, int depth);
    void addCheckeredParticleChunk();
    void addParticleDiagonal(int tiles, int numParticles);
    void addParticleDiagonal2(int tiles, int particleNum);
};

#endif
