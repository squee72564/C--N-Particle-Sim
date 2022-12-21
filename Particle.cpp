#include "Particle.hpp"

Particle::Particle(sf::Vector2f pos, sf::Vector2f vel, float m, std::mt19937& gen, std::uniform_int_distribution<>& dis)
    : position(pos), velocity(vel), mass(m)
{
    shape.setRadius(mass); // Set the radius of the circle to the mass of the particle
    shape.setFillColor(sf::Color(dis(gen), dis(gen), dis(gen))); //multicolored
    shape.setOrigin(mass, mass); // Set the origin of the circle to its center
}

Particle::~Particle() {}

void Particle::update(float dt, std::list<Particle>& particles)
{
    position += velocity * dt;
    shape.setPosition(position);

    // Check for collisions with other particles
    for (auto& other : particles)
    {
        if (&other == this) continue; // Skip self-collision

        // Calculate the distance between the two particles
        float distanceSquared = dot(position - other.position, position - other.position);

        // If the distance is smaller than the sum of the radii, the particles are colliding
        if (distanceSquared < (mass + other.mass) * (mass + other.mass))
        {
            // Handle collision here
            // Calculate the dot product of the velocity and position difference vectors
            float dotProduct1 = dot(velocity - other.velocity, position - other.position);
            float dotProduct2 = dot(other.velocity - velocity, other.position - position);

            // Multiply the reflection vector by a factor less than 1 to reduce its magnitude
            sf::Vector2f v1_new = velocity - REFLECTION_FACTOR * (2.0f * other.mass / (mass + other.mass)) * dotProduct1 * (position - other.position);
            sf::Vector2f v2_new = other.velocity - REFLECTION_FACTOR * (2.0f * mass / (mass + other.mass)) * dotProduct2 * (other.position - position);

            velocity = v1_new;
            other.velocity = v2_new;
        }
    }
}

sf::Vector2f& Particle::getVel()
{
    return velocity;
}

sf::Vector2f& Particle::getPos()
{
    return position;
}

sf::CircleShape& Particle::getShape()
{
    return shape;
}