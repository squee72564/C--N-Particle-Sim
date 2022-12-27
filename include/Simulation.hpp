#ifndef SIMULATION
#define SIMULATION
#include <SFML/Graphics.hpp> // Include the SFML graphics library

class Simulation
{
    protected:
        virtual void run() = 0;
        sf::RenderWindow* gameWindow;
};

#endif