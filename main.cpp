//g++ main.o Particle.o -o main -L"C:\SFML\lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
//g++ -c main.cpp Particle.cpp -I"C:/SFML/include" -I"C:/SFML/bin"

#include <string>
#include "Particle.hpp"

const float TIME_STEP = 0.01f;

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

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