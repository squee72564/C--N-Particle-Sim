#include "ParticleSimulation.hpp"
#include <iostream>

static sf::Vector2f getMousePosition(const sf::RenderWindow &window)
{
    // Get mouse position and convert to global coords 
    return window.mapPixelToCoords(sf::Mouse::getPosition(window));
}

template <typename T>
static inline float dot(const sf::Vector2<T>& vec1, const sf::Vector2<T>& vec2)
{
    return (vec1.x * vec2.x) + (vec1.y * vec2.y);
}

static inline float inv_Sqrt(float number)
{
    float squareRoot = sqrt(number);
    return 1.0f / squareRoot;
}

ParticleSimulation::ParticleSimulation(float dt, const sf::Vector2f& g, sf::RenderWindow &window, int nthreads, int treeDepth, int nodeCap, std::string logfile)
{
    gameWindow = &window;
    windowWidth = window.getSize().x;
    windowHeight = window.getSize().y;

    numThreads = nthreads;
    threads.reserve(nthreads);

    timeStep = dt;
    gravity = g;
    particleMass = 1.0f;
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

    isPausedText.setFont(font);
    isPausedText.setCharacterSize(24);
    isPausedText.setFillColor(sf::Color::White);
    isPausedText.setOutlineColor(sf::Color::Red);
    isPausedText.setOutlineThickness(1.0f);
    isPausedText.setString("Paused");
    isPausedText.setPosition(0, 50);

    std::cout << "Alloc Quadtree strating...\n";

    quadTree = QuadTree(windowWidth, windowHeight, treeDepth, nodeCap);

    std::cout << "Alloc Quadtree done...\n";

    isPaused = true;
    isRightButtonPressed = false;
    isAiming = false;
    showQuadTree = true;
    showVelocity = true;
    showParticles = true;

    iterationCount = 0;
    totalTime = 0;
    insertionTime = 0;
    leafnodeTime = 0;
    updateTime = 0;
    moveTime = 0;
    drawTime = 0;

    particles.reserve(5000);
    leafNodes.reserve(pow(4,treeDepth));

    logfileName = logfile;
}

ParticleSimulation::ParticleSimulation(float dt, const sf::Vector2f& g, sf::RenderWindow &window, int nthreads, int treeDepth, int nodeCap)
{
    gameWindow = &window;
    windowWidth = window.getSize().x;
    windowHeight = window.getSize().y;

    numThreads = nthreads;
    threads.reserve(nthreads);

    timeStep = dt;
    gravity = g;
    particleMass = 1.0f;
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

    isPausedText.setFont(font);
    isPausedText.setCharacterSize(24);
    isPausedText.setFillColor(sf::Color::White);
    isPausedText.setOutlineColor(sf::Color::Red);
    isPausedText.setOutlineThickness(1.0f);
    isPausedText.setString("Paused");
    isPausedText.setPosition(0, 50);


    std::cout << "Alloc Quadtree starting...\n";

    quadTree = QuadTree(windowWidth, windowHeight, treeDepth, nodeCap);

    std::cout << "Alloc Quadtree done...\n";

    isPaused = true;
    isRightButtonPressed = false;
    isAiming = false;
    showQuadTree = true;
    showVelocity = true;
    showParticles = true;

    iterationCount = 0;
    totalTime = 0;
    insertionTime = 0;
    leafnodeTime = 0;
    updateTime = 0;
    moveTime = 0;
    drawTime = 0;

    particles.reserve(8096);
    leafNodes.reserve(pow(4,treeDepth));
}

ParticleSimulation::~ParticleSimulation()
{
    if (!particles.empty())
        particles.clear();

    if (!leafNodes.empty())
        leafNodes.clear();

    if (!threads.empty())
        threads.clear();
}

void ParticleSimulation::run()
{
    iterationCount = 0;
    totalTime = 0.0;
    
    addParticleDiagonal(6, 50000);
    addParticleDiagonal(5, 50500);

    addParticleDiagonal2(6, 50000);
    addParticleDiagonal2(5, 50500);
    
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
                    if (particleMass < 10) { /**particleMass += 0.5;*/ }
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
                {
                    if (particleMass > 1) { /**particleMass -= 0.5;*/ }
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

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
                {
                    showParticles = (showParticles) ? false : true;
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
                    particles.emplace_back(Particle(initial_mousePosF, (initial_mousePosF-final_mousePosF), particleMass, gen , dis));
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

DEFINE_API_PROFILER(PopAndSwap);
DEFINE_API_PROFILER(InsertIntoQuadTree);
DEFINE_API_PROFILER(UpdateForces);
DEFINE_API_PROFILER(UpdateForces2);
DEFINE_API_PROFILER(DrawParticles);
DEFINE_API_PROFILER(DrawVelocities);
DEFINE_API_PROFILER(DrawQuadTree);

void ParticleSimulation::updateAndDraw()
{
    // Clear the window graphics
    gameWindow->clear();
    // Clear Quadtree
    quadTree.deleteTree();

    leafNodes.clear();

    // If LMB is pressed, create line for aim and show angle
    if (isRightButtonPressed || isAiming) {
        current_mousePosF = getMousePosition(*gameWindow);
    }

    {
    API_PROFILER(PopAndSwap);
    // Insert particles into QuadTree or erase if off screen
    for (std::size_t i = 0; i < particles.size(); ++i) {
        // Check if the particle's position is outside the window bounds
        if (particles[i].position.x < 0 || particles[i].position.x > windowWidth ||
            particles[i].position.y > windowHeight || particles[i].position.y < 0) {
            // If the particle is outside the window bounds, swap and pop from vector 
            std::swap(particles[i], particles.back());
            particles.pop_back();
            --i;
        }
    }
    }


    {
    API_PROFILER(InsertIntoQuadTree);
    for (std::size_t i = 0; i < particles.size(); ++i) {
        // Insert valid particle into QuadTree
        quadTree.insert(&particles[i], 0);
    }

    }

    // Traverse quadtree to get the leaf nodes
    globalCOM = quadTree.getLeafNodes(leafNodes, particles.size());

    if (!isPaused && !particles.empty()) {
        {
        API_PROFILER(UpdateForces);
        // This updates collision/gravity locally for each leaf node
        updateForces();
        }
    }
    

    if (!particles.empty() && showParticles) {

        {
        API_PROFILER(DrawParticles);
        sf::VertexArray points(sf::Points, particles.size());

        for (std::size_t i = 0; i < particles.size(); i++) {
            points[i].position = particles[i].position;
            points[i].color = particles[i].shape.getFillColor();

            // Create visual for particle's velocity vector if toggled
            if (showVelocity) {
                drawParticleVelocity(particles[i]);
            }
        }

        gameWindow->draw(points);
        }

        {
        API_PROFILER(DrawVelocities);

        for (std::size_t i = 0; i < particles.size(); i++) {
            // Create visual for particle's velocity vector if toggled
            if (showVelocity) {
                drawParticleVelocity(particles[i]);
            }
        }

        }


    }

    if (isAiming) {
        drawAimLine();
    }

    {
    API_PROFILER(DrawQuadTree);
    // Recursively draw QuadTree rectangles
    if (showQuadTree) {
        quadTree.display(gameWindow);
        sf::CircleShape circle(20.0f);
        circle.setOrigin(circle.getRadius(), circle.getRadius());
        circle.setPosition(globalCOM);
        circle.setFillColor(sf::Color(255,0,0,30));
        gameWindow->draw(circle);
    }

    }

    // Update the particle count & mass text
    particleCountText.setString("Particle count: " + std::to_string(particles.size()));
    particleMassText.setString("Particle mass: " + std::to_string( int(particleMass) ));
    
    // Draw the particle count & mass text
    gameWindow->draw(particleCountText);
    gameWindow->draw(particleMassText);

    if (isPaused) {
        gameWindow->draw(isPausedText);
    }
    // Display the window
    gameWindow->display();
}

inline void ParticleSimulation::drawAimLine() 
{	
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

inline void ParticleSimulation::drawParticleVelocity(Particle& particle) 
{
    // Create VertexArray from particle position to velocity vector of particle
    sf::VertexArray line(sf::Lines, 2);
    line[1].position.x = (particle.position.x + particle.velocity.x/200);
    line[1].position.y = (particle.position.y + particle.velocity.y/200);
    line[0].position = particle.position;
    line[0].color  = sf::Color(0,0,255,85);
    line[1].color = sf::Color(255,0,0,0);
   
    // Draw the velocity vector
    gameWindow->draw(line);
}

static inline void attractParticleToMousePos(Particle& particle, sf::Vector2f& current_mousePosF) 
{
    //particle.velocity -= sf::Vector2f(0.35f * (particle.position.x - current_mousePosF.x),
    //                            0.35f * (particle.position.y - current_mousePosF.y));

    //This provides a much softer attraction with RMB
    particle.acceleration -= sf::Vector2f(75.0f * (particle.position.x - current_mousePosF.x),
                                75.0f * (particle.position.y - current_mousePosF.y));
}

static inline void attractParticleToMousePos(Particle* particle, sf::Vector2f& current_mousePosF) 
{
    //particle.velocity -= sf::Vector2f(0.35f * (particle.position.x - current_mousePosF.x),
    //                            0.35f * (particle.position.y - current_mousePosF.y));

    //This provides a much softer attraction with RMB
    particle->acceleration -= sf::Vector2f(75.0f * (particle->position.x - current_mousePosF.x),
                                75.0f * (particle->position.y - current_mousePosF.y));
}

void ParticleSimulation::updateForces()
{
    //std::cout << "Updating forces for " << leafNodes.size() << " leaf nodes.\n";

    int n_threads = numThreads;

    if  (leafNodes.size() < static_cast<std::size_t>(numThreads)) {
        //std::cout << "\tSetting threads = " << leafNodes.size() <<"\n";
        numThreads = leafNodes.size();
    }

    const std::size_t chunkSize = leafNodes.size() / numThreads;
    const std::size_t remainder = leafNodes.size() % numThreads;


    for (int i = 0; i < numThreads; i++) {
        const std::size_t startIdx = i * chunkSize;
        const std::size_t endIdx = (i==numThreads-1) ? startIdx + chunkSize + remainder : startIdx + chunkSize;
        //std::cout << "\t\tStarting thread : " << i <<" to process leaf nodes [" << startIdx << "-" << endIdx << ")\n";
        
        // Create a lambda function that will be executed by each thread
        auto threadFunc = [this, startIdx, endIdx]() {
            for (std::size_t j = startIdx; j < endIdx; j++) {

                if (quadTree.empty(leafNodes[j]))
                    continue;

                const std::vector<Particle*>& leafNodeParticleVec = quadTree.getParticleVec(leafNodes[j]);
                
                for (Particle* particle : leafNodeParticleVec) {
                    for (Particle* other : leafNodeParticleVec) {

                        if (other == particle)
                            continue;

                        const float distanceSquared = dot(particle->position - other->position,
                                                    particle->position - other->position);

                        if (distanceSquared == 0)
                            continue;

                        const float radiusSquared = (particle->radius + other->radius) *
                                              (particle->radius + other->radius);

                        const bool is_colliding = (distanceSquared <= radiusSquared);
                        
                        if (is_colliding) {
                            //std::cout << "\t\t\tCOLLISION!!\n";
                            sf::Vector2f rHat = (other->position - particle->position) * inv_Sqrt(distanceSquared);

                            const float a1 = dot(particle->velocity, rHat);
                            const float a2 = dot(other->velocity, rHat);
                        
                            const float p = 2.0f * particle->mass * other->mass * (a1-a2)/(particle->mass + other->mass);
                            
                            particle->velocity -= p/particle->mass * (rHat);
                            other->velocity += p/other->mass * (rHat);

                        } else {

                            particle->acceleration += (other->mass / distanceSquared) * 
                                                        BIG_G * (other->position - particle->position);
                        }
                    }
                }
            }
        };

        threads.emplace_back(threadFunc);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    //std::cout << "Local calculations done.\n";
    threads.clear();

    for (int i = 0; i < numThreads; i++) {
        const std::size_t startIdx = i * chunkSize;
        const std::size_t endIdx = (i==numThreads-1) ? startIdx + chunkSize + remainder : startIdx + chunkSize;
        
        // Create a lambda function that will be executed by each thread
        auto threadFunc = [this, startIdx, endIdx]() {
            for (std::size_t j = startIdx; j < endIdx; j++) {

                const std::vector<Particle*>& leafNodeParticleVec = quadTree.getParticleVec(leafNodes[j]);
                
                // Use global com/total mass and local com/local mass to get the combined
                // gravitational force of all other leaf nodes but this current one
                sf::Vector2f newCOM(0,0);

                int nonLocalParticles = (particles.size() - leafNodeParticleVec.size());

                if (nonLocalParticles != 0) {

                    newCOM.x = static_cast<float>(particles.size() * globalCOM.x - quadTree.getCOM(leafNodes[j]).x) /
                                    static_cast<float>(particles.size() - leafNodeParticleVec.size());
                    newCOM.y = static_cast<float>(particles.size() * globalCOM.y - quadTree.getCOM(leafNodes[j]).y) /
                                    static_cast<float>(particles.size() - leafNodeParticleVec.size());
                }

                for (Particle* particle : leafNodeParticleVec) {
                    
                    if (nonLocalParticles != 0) {
                        const float distanceSquared = dot(particle->position - newCOM,
                                                      particle->position - newCOM);

                        particle->acceleration += (nonLocalParticles / distanceSquared) * BIG_G *
                                                        (newCOM - particle->position);
                    }

                    if (isRightButtonPressed) {
                        attractParticleToMousePos(particle, current_mousePosF);
                    }

                    particle->velocity += particle->acceleration * timeStep;
                    particle->position += particle->velocity * timeStep;
                    particle->acceleration.x = 0.0f;
                    particle->acceleration.y = 0.0f;
                }
            }
        };

        threads.emplace_back(threadFunc);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    threads.clear();

    numThreads = n_threads;
}

void ParticleSimulation::addParticleDiagonal(int tiles, int particleNum)
{
    const int col = sqrt(particleNum/tiles);
    const int row = col;

    const float smallWidth = static_cast<float>(windowWidth) / tiles;
    const float smallHeight = static_cast<float>(windowHeight) / tiles;

    for (int i = 0; i < tiles; i++) {
        for (int j = 0; j < col; j++) {
            for (int k = 0; k < row; k++ ) {
                const float x = (j * smallWidth / col) + smallWidth * i;
                const float y = (k * smallHeight / row) + smallHeight * i;
                particles.emplace_back(Particle(sf::Vector2f(x,y), sf::Vector2f(0,0), 1, gen , dis));
            }
        }
    }
}

void ParticleSimulation::addParticleDiagonal2(int tiles, int particleNum)
{
    const int col = sqrt(particleNum / tiles);
    const int row = col;

    const float smallWidth = static_cast<float>(windowWidth) / tiles;
    const float smallHeight = static_cast<float>(windowHeight) / tiles;

    for (int i = 0; i < tiles; i++) {
        for (int j = 0; j < col; j++) {
            for (int k = 0; k < row; k++) {
                const float x = static_cast<float>(windowWidth) - ((j * smallWidth / col) + smallWidth * i);
                const float y = (k * smallHeight / row) + smallHeight * i;

                particles.emplace_back(Particle(sf::Vector2f(x, y), sf::Vector2f(0, 0), 1, gen, dis));
            }
        }
    }
}

