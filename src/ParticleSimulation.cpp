#include "ParticleSimulation.hpp"

ParticleSimulation::ParticleSimulation(float dt, const sf::Vector2f& g, sf::RenderWindow &window)
{
    gameWindow = &window;
    WINDOW_WIDTH = window.getSize().x;
    WINDOW_HEIGHT = window.getSize().y;


    timeStep = dt;
    gravity = g;
    particleMass = 5.0f;
    gen = std::mt19937(rd());
    dis = std::uniform_int_distribution<>(0, 255);

    !font.loadFromFile("fonts/ALGER.TTF");

    particleCountText.setFont(font); // Set the font of the text
    particleCountText.setCharacterSize(24); // Set the size of the text
    particleCountText.setFillColor(sf::Color::White); // Set the fill color of the text
    particleCountText.setOutlineColor(sf::Color::Blue);
    particleCountText.setOutlineThickness(1.0f);

    particleMassText.setFont(font); // Set the font of the text
    particleMassText.setCharacterSize(12); // Set the size of the text
    particleMassText.setFillColor(sf::Color::White); // Set the fill color of the text
    particleMassText.setPosition(0, 100);
    particleMassText.setOutlineColor(sf::Color::Blue);
    particleMassText.setOutlineThickness(1.0f);
}

ParticleSimulation::~ParticleSimulation()
{
    if (!particles.empty())
    {
        particles.clear();
    }
}

void ParticleSimulation::run()
{
    // Run the program as long as the window is open
    while (gameWindow->isOpen())
    {
        pollUserEvent();
        updateAndDraw();
    }
}

void ParticleSimulation::pollUserEvent()
{
    // Check for events
    while (gameWindow->pollEvent(event))
    {
        switch (event.type) {
            case sf::Event::Closed:
                gameWindow->close(); // Close the window
                break;

            case sf::Event::KeyPressed:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
                {
                    if (particleMass < 10) { particleMass += 0.5; }
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
                {
                    if (particleMass > 1) { particleMass -= 0.5; }
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
                {
                    showInfo = (showInfo) ? false : true;
                }
                break;

            case sf::Event::MouseButtonReleased:
                if (isRightButtonPressed && !sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    isRightButtonPressed = false;
                }
                if (isAiming && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    isAiming = false;
                    final_mousePosF = getMousePostion(*gameWindow, final_mousePos);
                    particles.emplace_back(Particle(initial_mousePosF, initial_mousePosF-final_mousePosF, particleMass, gen , dis));
                }
                break;

            case sf::Event::MouseButtonPressed:
                if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    isRightButtonPressed = true;
                    current_mousePosF = getMousePostion(*gameWindow, current_mousePos);
                }
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    isAiming = true;
                    initial_mousePosF = getMousePostion(*gameWindow, initial_mousePos);
                }
                break;

            case sf::Event::MouseWheelScrolled:
                current_mousePosF = getMousePostion(*gameWindow, current_mousePos);

                // Add a new particle with 0 velocity
                particles.emplace_back(Particle(current_mousePosF, sf::Vector2f(0,0), particleMass, gen , dis));
                break;

            default:
                break;
        }
    }
}

void ParticleSimulation::updateAndDraw()
{
    gameWindow->clear(); // Clear the window

    // Update and draw all the particles
    for (auto it = particles.begin(); it != particles.end(); ++it)
    {
        // Check if the particle's position is outside the window bounds
        if (it->position.x < 0 || it->position.x > WINDOW_WIDTH || it->position.y > WINDOW_HEIGHT)
        {
            // If the particle is outside the window bounds, erase it from the list and continue
            it = particles.erase(it);
            continue;
        }

        it->velocity += gravity; // Apply gravity to the velocity

        // If RMG Pressed apply attractive force
        if (isRightButtonPressed)
        {
            current_mousePosF = getMousePostion(*gameWindow, current_mousePos);
            sf::Vector2f tempForce = sf::Vector2f(0.3 * (it->position.x - current_mousePosF.x), 0.3 * (it->position.y - current_mousePosF.y));
            it->velocity -= tempForce;
        }

        // Update the particle's position based on other particles O(n^2)
        it->update(timeStep, particles);

        gameWindow->draw(it->shape); // Draw the particle's shape

        // create visual for particle's velocity vector if toggled
        if (showInfo)  {
            sf::VertexArray line(sf::Lines, 2);
            line[1].position.x = (it->position.x + it->velocity.x/30);
            line[1].position.y = (it->position.y + it->velocity.y/30);
            line[0].position = it->position;
            line[0].color  = sf::Color::Red;
            line[1].color = sf::Color::Blue;
            
            gameWindow->draw(line); // Draw the velocity vector
        }
    }

    // If LMB is pressed, create line for aim
    if (isAiming) {
        current_mousePosF = getMousePostion(*gameWindow, current_mousePos);

        sf::VertexArray line(sf::Lines, 2);
        line[0].position = initial_mousePosF;
        line[1].position = current_mousePosF;
        line[0].color  = sf::Color(0, 255, 0, 255);
        line[1].color = sf::Color(0, 255, 0, 55);
        
        gameWindow->draw(line);
    }
    
    // Update the particle count & mass text
    particleCountText.setString("Particle count: " + std::to_string(particles.size()));
    particleMassText.setString("Particle mass: " + std::to_string( int(particleMass) ));

    // Draw the particle count & mass text
    gameWindow->draw(particleCountText);
    gameWindow->draw(particleMassText);

    gameWindow->display(); // Display the window
}

sf::Vector2f getMousePostion(const sf::RenderWindow &window, sf::Vector2i &mousePos)
{
    // Get the mouse position
    mousePos = sf::Mouse::getPosition(window);
    // Convert the mouse position to a float vector
    return sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}