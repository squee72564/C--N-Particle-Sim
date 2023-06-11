#include "ParticleSimulation.hpp"

ParticleSimulation::ParticleSimulation(float dt, const sf::Vector2f& g, sf::RenderWindow &window, int threads)
{
    gameWindow = &window;
    windowWidth = window.getSize().x;
    windowHeight = window.getSize().y;

    numThreads = threads;

    timeStep = dt;
    gravity = g;
    particleMass = 5.0f;
    gen = std::mt19937(rd());
    dis = std::uniform_int_distribution<>(0, 255);

    font.loadFromFile("fonts/corbel.TTF");

    particleCountText.setFont(font);
    particleCountText.setCharacterSize(24);
    particleCountText.setFillColor(sf::Color::White);
    particleCountText.setOutlineColor(sf::Color::Blue);
    particleCountText.setOutlineThickness(1.0f);

    particleMassText.setFont(font);
    particleMassText.setCharacterSize(12);
    particleMassText.setFillColor(sf::Color::White);
    particleMassText.setPosition(0, 100);
    particleMassText.setOutlineColor(sf::Color::Blue);
    particleMassText.setOutlineThickness(1.0f);

    velocityText.setFont(font);
    velocityText.setCharacterSize(10);
    velocityText.setFillColor(sf::Color::White);

    quadTree = QuadTree(0, windowWidth, windowHeight);

    isRightButtonPressed = false;
    isAiming = false;
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
        switch (event.type)
        {
            case sf::Event::Closed:	// Window Closed
                gameWindow->close();
                break;

            case sf::Event::KeyPressed:	// Key press
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
                    showVelocity = (showVelocity) ? false : true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
                {
                    showQuadTree = (showQuadTree) ? false : true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                {
                    isPaused = (isPaused) ? false : true;
                }

                break;

            case sf::Event::MouseButtonReleased: // RMB or LMB released

                if (isRightButtonPressed && !sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    isRightButtonPressed = false;
                }

                if (isAiming && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    isAiming = false;
                    final_mousePosF = getMousePosition(*gameWindow);
                    particles.emplace_back(Particle(initial_mousePosF, initial_mousePosF-final_mousePosF, particleMass, gen , dis));
                }

                break;

            case sf::Event::MouseButtonPressed:	// RMB or LMB pressed
						
                if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    isRightButtonPressed = true;
                    current_mousePosF = getMousePosition(*gameWindow);
                }

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    isAiming = true;
                    initial_mousePosF = getMousePosition(*gameWindow);
                }

                break;

            case sf::Event::MouseWheelScrolled:	// Emplace particle at mouse position
						
                current_mousePosF = getMousePosition(*gameWindow);

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
    // Clear the window graphics
    gameWindow->clear();
    // Clear Quadtree
    quadTree.deleteTree();

    // If LMB is pressed, create line for aim and show angle
    if (isAiming)
    {
        drawAimLine();
    }
    
    // Update the particle count & mass text
    particleCountText.setString("Particle count: " + std::to_string(particles.size()));
    particleMassText.setString("Particle mass: " + std::to_string( int(particleMass) ));
    
    // Draw the particle count & mass text
    gameWindow->draw(particleCountText);
    gameWindow->draw(particleMassText);

    // Insert particles into QuadTree or erase if off screen
    for (std::size_t i = 0; i < particles.size(); i++)
    {
        // Check if the particle's position is outside the window bounds
        if (particles[i].position.x < 0 || particles[i].position.x > windowWidth ||
			particles[i].position.y > windowHeight || particles[i].position.y < 0)
        {
            // If the particle is outside the window bounds, swap and pop from vector 
	        std::swap(particles[i], particles.back());
	        particles.pop_back();
            i--;
        } else {
            // Insert valid particle into QuadTree
            quadTree.insert(&particles[i]);
        }
    }

    // Split the particles into equal-sized chunks for each thread
    const std::size_t chunkSize = particles.size() / numThreads;
    const std::size_t remainder = particles.size() % numThreads;

    // Create a vector to store the threads
    std::vector<std::thread> threads;

    // Create and start the threads
    for (int i = 0; i < numThreads; i++)
    {
        // Define the start and end indices for the current chunk
        std::size_t startIdx = i * chunkSize;
        std::size_t endIdx = startIdx + chunkSize;

        // If it's the last thread, adjust the end index to include the remainder particles
        if (i == numThreads - 1)
        {
            endIdx += remainder;
        }

        // Create a lambda function that will be executed by each thread
        auto threadFunc = [this, startIdx, endIdx]() {
            for (std::size_t j = startIdx; j < endIdx; j++) {
                if (!isPaused) {
                    // Apply gravity to the velocity
                    this->particles[j].velocity += this->gravity;

                    // Update position of particle based on Quadtree
                    this->quadTree.update(this->timeStep, &(this->particles[j]));

                    // If RMB Pressed, apply attractive force
                    if (this->isRightButtonPressed) {
                        attractParticleToMousePos(this->particles[j]);
                    }
                }
            }
        };

        // Create a thread and pass the lambda function
        threads.emplace_back(threadFunc);
    }

    // Wait for all threads to finish
    for (auto& thread : threads)
    {
        thread.join();
    }

    for (std::size_t i = 0; i < particles.size(); i++)
    {

	    // Set particle circle shape to new position
        particles[i].shape.setPosition(particles[i].position);
	
	    // Draw the particle's shape
        gameWindow->draw(particles[i].shape); 

        // Create visual for particle's velocity vector if toggled
        if (showVelocity)
        {
	        drawParticleVelocity(particles[i]);
        }
    }

    // Recursively draw QuadTree rectangles
    if (showQuadTree)
    {
        quadTree.display(gameWindow);
    }
    
    // Display the window
    gameWindow->display(); 
}

void ParticleSimulation::drawAimLine() 
{	
    // Get current mouse position
    current_mousePosF = getMousePosition(*gameWindow);

    // Setting text position and value for angle
    velocityText.setPosition(initial_mousePosF.x+5, initial_mousePosF.y);
    float t =  (initial_mousePosF.y - current_mousePosF.y) / (initial_mousePosF.x - current_mousePosF.x);
    velocityText.setString(std::to_string( abs( ( atan(t) * 180 ) / 3.14) ));

    // Create VertexArray from initial to current position
    sf::VertexArray line(sf::Lines, 2);
    line[0].position = initial_mousePosF;
    line[1].position = current_mousePosF;
    line[0].color  = sf::Color(0, 255, 0, 155);
    line[1].color = sf::Color(0, 255, 0, 25);
        
    // Draw VertexArray and text to screen
    gameWindow->draw(velocityText);
    gameWindow->draw(line);
}

void ParticleSimulation::drawParticleVelocity(Particle& particle) 
{
    // Create VertexArray from particle position to velocity vector of particle
    sf::VertexArray line(sf::Lines, 2);
    line[1].position.x = (particle.position.x + particle.velocity.x/50);
    line[1].position.y = (particle.position.y + particle.velocity.y/50);
    line[0].position = particle.position;
    line[0].color  = sf::Color(0,0,255,255);
    line[1].color = sf::Color(255,0,0,0);
   
    // Draw the velocity vector
    gameWindow->draw(line);
}

void ParticleSimulation::attractParticleToMousePos(Particle& particle) 
{
    // Get current mouse position
    current_mousePosF = getMousePosition(*gameWindow);

    // Create force vector from particle to mouse
    sf::Vector2f tempForce = sf::Vector2f(0.4 * (particle.position.x - current_mousePosF.x),
		    0.4 * (particle.position.y - current_mousePosF.y));

    // Apply attractive force to particle velocity
    particle.velocity -= tempForce;
}

sf::Vector2f getMousePosition(const sf::RenderWindow &window)
{
    // Get mouse position and convert to global coords 
    return window.mapPixelToCoords(sf::Mouse::getPosition(window));
}
