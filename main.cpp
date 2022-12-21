#include <SFML/Graphics.hpp> // Include the SFML graphics library
#include <list> // Include the vector container
#include <string>
#include <random>

const float REFLECTION_FACTOR = 0.02f;
const float TIME_STEP = 0.01f;

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

template <typename T>
float dot(const sf::Vector2<T>& vec1, const sf::Vector2<T>& vec2);

// Particle structure
struct Particle
{
    sf::Vector2f position; // Position of the particle
    sf::Vector2f velocity; // Velocity of the particle
    sf::CircleShape shape; // Circle shape to represent the particle
    float mass; // Mass of the particle

    // Constructor to initialize the particle's properties
    Particle(sf::Vector2f pos, sf::Vector2f vel, float m, std::mt19937& gen, std::uniform_int_distribution<>& dis)
        : position(pos), velocity(vel), mass(m)
    {
        shape.setRadius(mass); // Set the radius of the circle to the mass of the particle
        shape.setFillColor(sf::Color(dis(gen), dis(gen), dis(gen))); //multicolored
        shape.setOrigin(mass, mass); // Set the origin of the circle to its center
    }

    void update(float dt, std::list<Particle>& particles)
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
};

int main()
{
    // Create the window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Particle Simulator");
    window.setFramerateLimit(60); // Limit the frame rate to 60 FPS

    // Vector to store the particles
    std::list<Particle> particles;

    // Gravity vector
    sf::Vector2f gravity(0, 0.981f);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    // Create a font
    sf::Font font;
    if (!font.loadFromFile("C:/Users/recru/Exilent/src/fonts/tahoma.ttf"))
    {
        return -1;
    }

    // Create a text object to display the particle count
    sf::Text particleCountText;
    particleCountText.setFont(font); // Set the font of the text
    particleCountText.setCharacterSize(24); // Set the size of the text
    particleCountText.setFillColor(sf::Color::White); // Set the fill color of the text

    // Run the program as long as the window is open
    while (window.isOpen())
    {
        sf::Event event;
        // Check for events
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) // If the event is the window being closed
            {
                window.close(); // Close the window
            }
            else if (event.type == sf::Event::MouseWheelScrolled || (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)) // If the event is the left mouse button being pressed
            {
                // Get the mouse position
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                // Convert the mouse position to a float vector
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                // Add a new particle with a random velocity
                particles.emplace_back(mousePosF, sf::Vector2f(rand() % 200 - 100, -(rand() % 100 + 50)), 5.0f, gen , dis);
            }
        }

        window.clear(); // Clear the window

        // Update and draw all the particles
        for (auto it = particles.begin(); it != particles.end();)
        {
            // Apply gravity to the velocity
            it->velocity += gravity;
            // Update the particle's position and shape
            it->update(TIME_STEP, particles);
            window.draw(it->shape); // Draw the particle's shape

            // Check if the particle's position is outside the window bounds
            if (it->position.x < 0 || it->position.x > WINDOW_WIDTH || it->position.y > WINDOW_HEIGHT)
            {
                // If the particle is outside the window bounds, erase it from the vector
                it = particles.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // Update the particle count text
        particleCountText.setString("Particle count: " + std::to_string(particles.size()));

        // Draw the particle count text
        window.draw(particleCountText);

        window.display(); // Display the window
    }
        return 0;
}

template <typename T>
float dot(const sf::Vector2<T>& vec1, const sf::Vector2<T>& vec2)
{
    return (vec1.x * vec2.x) + (vec1.y * vec2.y);
}