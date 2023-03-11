#ifndef PARTICLE
#define PARTICLE
#include <random>
#include <SFML/Graphics.hpp> // Include the SFML graphics library
#include <math.h>

static const float REFLECTION_FACTOR = 0.015f;
static const float BIG_G = 2000.0f;

class Particle
{
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float mass;
    float radius;
    sf::CircleShape shape;
    sf::Vector2f acceleration;

    Particle();
    Particle(const sf::Vector2f pos, const sf::Vector2f vel, float m, std::mt19937& gen, std::uniform_int_distribution<>& dis);
    Particle(const Particle& particle);
    ~Particle();
};

#endif
