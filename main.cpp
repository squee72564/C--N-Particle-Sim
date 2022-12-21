#include <SFML/Graphics.hpp> // Include the SFML graphics library
#include <list> // Include the vector container
#include <string>
#include <random>

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

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

    // Update the particle's position and shape
    void update(float dt)
    {
        position += velocity * dt; // Update the position using the velocity and the time step
        shape.setPosition(position); // Set the position of the shape to the position of the particle
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

        // Update all the particles
        for (auto it = particles.begin(); it != particles.end();)
        {
            // Apply gravity to the velocity
            it->velocity += gravity;
            // Update the particle's position and shape
            it->update(1.0f / 60.0f);

            // Check if the particle's position is outside the window bounds
            if (it->position.x < 0 || it->position.x > WINDOW_WIDTH || it->position.y > WINDOW_HEIGHT)
            {
                // If the particle is outside the window bounds, remove it from the vector
                it = particles.erase(it);
            }
            else
            {
                ++it;
            }
        }

        window.clear(); // Clear the window

        // Draw all the particles
        for (const auto& particle : particles)
        {
            window.draw(particle.shape); // Draw the particle's shape
        }

        // Update the particle count text
        particleCountText.setString("Particle count: " + std::to_string(particles.size()));

        // Draw the particle count text
        window.draw(particleCountText);

        window.display(); // Display the window
    }
        return 0;
}