#ifndef PARTICLE
#define PARTICLE

#include <SFML/Graphics.hpp>

class Particle
{
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    sf::Color color;
    float mass;

    Particle();
    Particle(const sf::Vector2f& pos, const sf::Vector2f& vel, float m);
    Particle(const Particle& particle);
    Particle(Particle&& particle);
    Particle& operator=(const Particle& particle);
    Particle& operator=(Particle&& particle);
    ~Particle();
};

#endif
