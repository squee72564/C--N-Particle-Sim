#include "Particle.hpp"

Particle::Particle()
    : position(sf::Vector2f(0,0)),
      velocity(sf::Vector2f(0,0)),
      mass(1)
{
    radius = 1;
    shape.setRadius(radius); // Set the radius of the circle to the mass of the particle
    shape.setFillColor(sf::Color(255, 255, 255)); //white
    shape.setOrigin(mass, mass); // Set the origin of the circle to its center
}

Particle::Particle(sf::Vector2f pos, sf::Vector2f vel, float m, std::mt19937& gen, std::uniform_int_distribution<>& dis)
    : position(pos),
      velocity(vel),
      mass(m)
{
    radius = m;
    shape.setRadius(radius); // Set the radius of the circle to the mass of the particle
    shape.setFillColor(sf::Color(dis(gen), dis(gen), dis(gen))); //multicolored
    shape.setOrigin(mass, mass); // Set the origin of the circle to its center
}

Particle::Particle(const Particle& particle)
    : position(particle.position),
      velocity(particle.velocity),
      mass(particle.mass)
{
    radius = mass;
    shape.setRadius(radius); // Set the radius of the circle to the mass of the particle
    shape.setFillColor(particle.shape.getFillColor()); //multicolored
    shape.setOrigin(mass, mass); // Set the origin of the circle to its center
}

Particle::~Particle() {}