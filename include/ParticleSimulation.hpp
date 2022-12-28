#ifndef PARTICLE_SIMULATION
#define PARTICLE_SIMULATION

#include <string>
#include <iostream>
#include <list>
#include <stack>
#include "Simulation.hpp"
#include "Particle.hpp"
#include "Quadtree.hpp"

class ParticleSimulation : Simulation
{
private:
    // Game Window
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;

    // Delta Time
    float timeStep;

    QuadTree quadTree;

    std::list<Particle> particles;

    sf::Vector2f gravity;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dis;

    bool isRightButtonPressed;
    sf::Vector2i current_mousePos;
    sf::Vector2f current_mousePosF;

    bool isAiming;
    sf::Vector2i initial_mousePos;
    sf::Vector2f initial_mousePosF;
    sf::Vector2i final_mousePos;
    sf::Vector2f final_mousePosF;

    bool showInfo;

    float particleMass;

    sf::Font font;
    sf::Text particleCountText;
    sf::Text particleMassText;

    sf::Event event;

public:
    ParticleSimulation(float dt, const sf::Vector2f &g, sf::RenderWindow &window);
    ParticleSimulation(ParticleSimulation& sim);
    virtual ~ParticleSimulation();
    void run();
    void pollUserEvent();
    void updateAndDraw();
};

sf::Vector2f getMousePostion(const sf::RenderWindow &window, sf::Vector2i &mousePos);

#endif