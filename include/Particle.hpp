#ifndef PARTICLE
#define PARTICLE
#include <random>
#include <list> // Include the vector container
#include <SFML/Graphics.hpp> // Include the SFML graphics library
#include <math.h>

static const float REFLECTION_FACTOR = 0.015f;
static const float BIG_G = 1000.0f;

class Particle
{
public:
    sf::Vector2f position; // Position of the particle
    sf::Vector2f velocity; // Velocity of the particle
    float mass; // Mass of the particle
    float radius;
    sf::CircleShape shape; // Circle shape to represent the particle
   
    Particle();
    Particle(sf::Vector2f pos, sf::Vector2f vel, float m, std::mt19937& gen, std::uniform_int_distribution<>& dis);
    Particle(const Particle& particle);
    ~Particle();

    void update(float dt, std::list<Particle>& particles);
    void checkCollision(std::list<Particle>& particles);
};


template <typename T>
inline float dot(const sf::Vector2<T>& vec1, const sf::Vector2<T>& vec2);

inline float inv_Sqrt(float number);

#endif