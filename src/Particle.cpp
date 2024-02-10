#include "Particle.hpp"

Particle::Particle()
    : position(sf::Vector2f(0,0)),
      velocity(sf::Vector2f(0,0)),
      acceleration(sf::Vector2f(0,0)),
      mass(1),
      radius(0.5),
      index(-1) {}

Particle::Particle(const sf::Vector2f& pos, const sf::Vector2f& vel, float m, int index)
    : position(pos),
      velocity(vel),
      acceleration(sf::Vector2f(0,0)),
      mass(m),
      radius(0.5),
      index(index) {}

Particle::Particle(const Particle& particle)
    : position(particle.position),
      velocity(particle.velocity),
      acceleration(particle.acceleration),
      mass(particle.mass),
      radius(particle.radius),
      index(particle.index) {}

Particle::Particle(Particle&& particle)
    : position(std::move(particle.position)),
      velocity(std::move(particle.velocity)),
      acceleration(std::move(particle.acceleration)),
      mass(particle.mass),
      radius(particle.radius),
      index(particle.index) {}

Particle& Particle::operator=(const Particle& particle)
{
    if (this != &particle) {
        position = particle.position;
        velocity = particle.velocity;
        acceleration = particle.acceleration;
        mass = particle.mass;
        radius = particle.radius;
        index = particle.index;
    }
    
    return *this;
}

Particle& Particle::operator=(Particle&& particle)
{
    if (this != &particle) {
        position = std::move(particle.position);
        velocity = std::move(particle.velocity);
        acceleration = std::move(particle.acceleration);
        mass = particle.mass;
        radius = particle.radius;
        index = particle.index;
    }
    
    return *this;
}

Particle::~Particle() {}
