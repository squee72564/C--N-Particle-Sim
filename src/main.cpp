//g++ main.o Particle.o -o main -L"C:\SFML\lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
//g++ -c main.cpp Particle.cpp -I"C:/SFML/include" -I"C:/SFML/bin"

#include <string>
#include <iostream>
#include "Particle.hpp"

const float TIME_STEP = 0.004f;

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
    //std::vector<Particle> particles;

    // Gravity vector
    //sf::Vector2f gravity(0, 0.981f);
    sf::Vector2f gravity(0.0f, 0.0f);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    bool isRightButtonPressed = false;
    sf::Vector2i current_mousePos;
    sf::Vector2f current_mousePosF;

    bool isAiming = false;
    sf::Vector2i initial_mousePos;
    sf::Vector2f initial_mousePosF;
    sf::Vector2i final_mousePos;
    sf::Vector2f final_mousePosF;

    bool showInfo = false;

    float particleMass = 5.0f;

    // Create a font
    sf::Font font;
    if (!font.loadFromFile("fonts/ALGER.TTF"))
    {
        return -1;
    }

    // Create a text object to display the particle count
    sf::Text particleCountText;
    particleCountText.setFont(font); // Set the font of the text
    particleCountText.setCharacterSize(24); // Set the size of the text
    particleCountText.setFillColor(sf::Color::White); // Set the fill color of the text
    particleCountText.setOutlineColor(sf::Color::Blue);
    particleCountText.setOutlineThickness(1.0f);
    // Create a text object to display the particle mass
    sf::Text particleMassText;
    particleMassText.setFont(font); // Set the font of the text
    particleMassText.setCharacterSize(12); // Set the size of the text
    particleMassText.setFillColor(sf::Color::White); // Set the fill color of the text
    particleMassText.setPosition(0, 100);
    particleMassText.setOutlineColor(sf::Color::Blue);
    particleMassText.setOutlineThickness(1.0f);

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
            else if(event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            {
                if (particleMass < 10)
                {
                    particleMass += 0.5;
                }
            }
            else if(event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::X))
            {
                if (particleMass > 1)
                {
                    particleMass -= 0.5;
                }
            }
            else if(event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::I))
            {
                if (showInfo) {
                    showInfo = false;
                } else {
                    showInfo = true;
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
            {
                isAiming = false;
                // Get the mouse position
                final_mousePos = sf::Mouse::getPosition(window);
                // Convert the mouse position to a float vector
                final_mousePosF = sf::Vector2f(static_cast<float>(final_mousePos.x), static_cast<float>(final_mousePos.y));
                particles.emplace_back(initial_mousePosF, initial_mousePosF-final_mousePosF, particleMass, gen , dis);

            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                isAiming = true;
                // Get the mouse position
                initial_mousePos = sf::Mouse::getPosition(window);
                // Convert the mouse position to a float vector
                initial_mousePosF = sf::Vector2f(static_cast<float>(initial_mousePos.x), static_cast<float>(initial_mousePos.y));

                std::cout << "Pressed\n";
            }
            else if (event.type == sf::Event::MouseWheelScrolled) // If the event is the left mouse button being pressed
            {
                // Get the mouse position
                current_mousePos = sf::Mouse::getPosition(window);
                // Convert the mouse position to a float vector
                current_mousePosF = sf::Vector2f(static_cast<float>(current_mousePos.x), static_cast<float>(current_mousePos.y));

                // Add a new particle with a random velocity
                //particles.emplace_back(current_mousePosF, sf::Vector2f(rand() % 25 - 25, -(rand() % 25 + 25)), particleMass, gen , dis);
                particles.emplace_back(current_mousePosF, sf::Vector2f(0,0), particleMass, gen , dis);
            }
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                if (!sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    isRightButtonPressed = false;
                }
                
            }
            else if (isRightButtonPressed || (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right))
            {
                isRightButtonPressed = true;
                // Get the mouse position
                current_mousePos = sf::Mouse::getPosition(window);
                // Convert the mouse position to a float vector
                current_mousePosF = sf::Vector2f(static_cast<float>(current_mousePos.x), static_cast<float>(current_mousePos.y));
            }
        }

        window.clear(); // Clear the window

        // Update and draw all the particles
        for (auto it = particles.begin(); it != particles.end(); ++it)
        {

            // Check if the particle's position is outside the window bounds
            if (it->position.x < 0 || it->position.x > WINDOW_WIDTH || it->position.y > WINDOW_HEIGHT)
            {
                // If the particle is outside the window bounds, erase it from the vector
                it = particles.erase(it);
                continue;
            }

            // Apply gravity to the velocity
            it->velocity += gravity;

            if (isRightButtonPressed)
            {
                sf::Vector2f tempForce = sf::Vector2f(0.3 * (it->position.x - current_mousePosF.x), 0.3 * (it->position.y - current_mousePosF.y));

                // Add the force to the total force applied to the particle
                it->velocity -= tempForce;
            }
 
            // Update the particle's gravitational forces and collision
            it->update(TIME_STEP, particles);

            window.draw(it->shape); // Draw the particle's shape

            if (showInfo)  {
                // create line for particle's velocity vector
                sf::VertexArray line(sf::Lines, 2);
                line[1].position.x = (it->position.x + it->velocity.x/30);
                line[1].position.y = (it->position.y + it->velocity.y/30);
                line[0].position = it->position;
                line[0].color  = sf::Color::Red;
                line[1].color = sf::Color::Blue;
                
                window.draw(line); // Draw the velocity vector
            }
        }

        if (isAiming) {
            current_mousePos = sf::Mouse::getPosition(window);
            current_mousePosF = sf::Vector2f(static_cast<float>(current_mousePos.x), static_cast<float>(current_mousePos.y));

            // create line for particle's velocity vector
            sf::VertexArray line(sf::Lines, 2);
            line[0].position = initial_mousePosF;
            line[1].position = current_mousePosF;
            line[0].color  = sf::Color(0, 255, 0, 255);
            line[1].color = sf::Color(0, 255, 0, 55);
            
            window.draw(line); // Draw the velocity vector
        }
        
        // Update the particle count text
        particleCountText.setString("Particle count: " + std::to_string(particles.size()));
        particleMassText.setString("Particle mass: " + std::to_string( int(particleMass) ));

        // Draw the particle count text
        window.draw(particleCountText);
        window.draw(particleMassText);
        window.display(); // Display the window
    }
    
    return 0;
}