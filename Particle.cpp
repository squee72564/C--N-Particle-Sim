#include "Particle.hpp"

float inv_Sqrt(float number);

Particle::Particle(sf::Vector2f pos, sf::Vector2f vel, float m, std::mt19937& gen, std::uniform_int_distribution<>& dis)
    : position(pos), velocity(vel), mass(m)
{
    radius = m;
    shape.setRadius(radius); // Set the radius of the circle to the mass of the particle
    shape.setFillColor(sf::Color(dis(gen), dis(gen), dis(gen))); //multicolored
    shape.setOrigin(mass, mass); // Set the origin of the circle to its center
}

Particle::~Particle() {}

void Particle::update(float dt, std::list<Particle>& particles)
{
    // Check for collisions with other particles
    for (auto& other : particles)
    {
        if (&other == this) continue; // Skip self-collision

        // Calculate the distance squared between the two particles as a scalar.
        float distanceSquared = dot(position - other.position, position - other.position);
        
        // If the distance is greater than the sum of the radii then the particles can attract
        if (distanceSquared != 0) {
            // Calculate acceleration due to force of attraction to other particle.
            float acceleration = (other.mass / distanceSquared) * BIG_G;

            // Incrementing velocity by the acceleration due to force of attraction between particles.
            velocity += (acceleration * dt) * (other.position - position);
        }

        // If the distance is smaller than the sum of the radii, the particles are colliding
        if (distanceSquared != 0 && distanceSquared <= (radius + other.radius) * (radius + other.radius))
        {
            // Handle collision here
            // Normalized point of contact vector
            sf::Vector2f rHat = (other.position - position) * inv_Sqrt(distanceSquared);

            // Component for current particle velocity along rHat direction
            float a1 = dot(velocity, rHat);
             // Component for other particle velocity along rHat direction
            float a2 = dot(other.velocity, rHat);
            
            // Momentum change due to collision
            float p = 2 * (a1-a2)/(mass + other.mass);

            velocity -= p * other.mass * (rHat);
            other.velocity += p * mass * (rHat); 
        }
    }

    position += velocity * dt;
    shape.setPosition(position);
}

float inv_Sqrt(float number)
{
    float squareRoot = sqrt(number);
    return 1.0f / squareRoot;
}