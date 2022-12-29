#include "Particle.hpp"

Particle::Particle()
    : position(sf::Vector2f(0,0)),
      velocity(sf::Vector2f(0,0)),
      mass(1),
      id(++PARTICLE_ID % 2000)
{
    radius = 1;
    shape.setRadius(radius); // Set the radius of the circle to the mass of the particle
    shape.setFillColor(sf::Color(255, 255, 255)); //white
    shape.setOrigin(mass, mass); // Set the origin of the circle to its center
}

Particle::Particle(sf::Vector2f pos, sf::Vector2f vel, float m, std::mt19937& gen, std::uniform_int_distribution<>& dis)
    : position(pos),
      velocity(vel),
      mass(m),
      id(++PARTICLE_ID % 2000)
{
    radius = m;
    shape.setRadius(radius); // Set the radius of the circle to the mass of the particle
    shape.setFillColor(sf::Color(dis(gen), dis(gen), dis(gen))); //multicolored
    shape.setOrigin(mass, mass); // Set the origin of the circle to its center
}

Particle::Particle(const Particle& particle)
    : position(particle.position),
      velocity(particle.velocity),
      mass(particle.mass),
      id(particle.id)
{
    radius = mass;
    shape.setRadius(radius); // Set the radius of the circle to the mass of the particle
    shape.setFillColor(particle.shape.getFillColor()); //multicolored
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
        if (distanceSquared != 0 && distanceSquared > (radius + other.radius) * (radius + other.radius)) {
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
            float p = 2 * mass * other.mass * (a1-a2)/(mass + other.mass);

            velocity -= p/mass * (rHat);
            other.velocity += p/other.mass * (rHat); 
        }
    }

    position += velocity * dt;
}

void Particle::checkCollision(std::list<Particle>& particles)
{
    for (auto& other : particles)
    {
        if(this == &other)
        {
            continue;
        }
        
        // Calculate the distance squared between the two particles as a scalar.
        float distanceSquared = dot(position - other.position, position - other.position);

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
            float p = 2 * mass * other.mass * (a1-a2)/(mass + other.mass);

            velocity -= p/mass * (rHat);
            other.velocity += p/other.mass * (rHat); 
        }
    }

}


template <typename T>
float dot(const sf::Vector2<T>& vec1, const sf::Vector2<T>& vec2)
{
    return (vec1.x * vec2.x) + (vec1.y * vec2.y);
}

float inv_Sqrt(float number)
{
    float squareRoot = sqrt(number);
    return 1.0f / squareRoot;
}