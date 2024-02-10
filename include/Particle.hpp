#ifndef PARTICLE
#define PARTICLE
#include <random>
#include <SFML/Graphics.hpp> // Include the SFML graphics library
#include <math.h>


class Particle
{
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float mass;
    float radius;
    int index;

    Particle();
    Particle(const sf::Vector2f& pos, const sf::Vector2f& vel, float m, int index);
    Particle(const Particle& particle);
    Particle(Particle&& particle);
    Particle& operator=(const Particle& particle);
    Particle& operator=(Particle&& particle);
    ~Particle();
};

#endif
