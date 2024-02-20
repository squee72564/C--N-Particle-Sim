#include "ParticleSimulation.hpp"
#include <iostream>

//static const float REFLECTION_FACTOR = 0.010f;
static const float BIG_G = 35.00f;

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

ParticleSimulation::ParticleSimulation(int simulationWidth,
                                       int simulationHeight,
                                       sf::RenderWindow &window,
                                       int numThreads,
                                       float dt,
                                       const sf::Vector2f& g,
                                       int treeDepth,
                                       int nodeCap)
  : numThreads(numThreads),
    treeMaxDepth(treeDepth),
    simulationWidth(simulationWidth),
    simulationHeight(simulationHeight),
    gameView(sf::Vector2f(window.getSize().x/2, window.getSize().y/2), sf::Vector2f(window.getSize().x, window.getSize().y)),
    gen(std::mt19937(rd())),
    dis(std::uniform_int_distribution<>(0, 255)),
    timeStep(dt),
    particleMass(1.03f),
    gravity(g),
    globalCOM(sf::Vector2f(0.0f, 0.0f)),
    current_mousePosF(sf::Vector2f(0.0f, 0.0f)),
    initial_mousePosF(sf::Vector2f(0.0f, 0.0f)),
    final_mousePosF(sf::Vector2f(0.0f, 0.0f)),
    isRightButtonPressed(false),
    isMiddleButtonPressed(false),
    isAiming(false),
    showVelocity(false),
    showQuadTree(true),
    showParticles(true),
    isPaused(true),
    font(),
    threads(),
    leafNodes(),
    particles(),
    quadTree(QuadTree(simulationWidth, simulationHeight, treeDepth, nodeCap))
{
    gameWindow = &window;
    gameWindow->setView(gameView);

    font.loadFromFile("fonts/corbel.TTF");
    threads.reserve(numThreads);
    leafNodes.reserve(pow(4,treeDepth)+1);
    particles.reserve(100000);

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
    addCheckeredParticleChunk();

    //addSierpinskiTriangleParticleChunk((simulationWidth-simulationHeight)/2, 0, simulationHeight, 10);
    
    while (gameWindow->isOpen())
    {
        pollUserEvent();
        updateAndDraw();
    }
}

void ParticleSimulation::pollUserEvent()
{
    while (gameWindow->pollEvent(event))
    {
        int scrollDelta = 0;

        switch (event.type)
        {
            case sf::Event::Closed:
                gameWindow->close();
                break;

            case sf::Event::KeyPressed:
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
                {
                    if (quadTree.getMaxDepth() > 0) quadTree.setMaxDepth(quadTree.getMaxDepth()-1);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
                {
                    if (quadTree.getMaxDepth() < treeMaxDepth) quadTree.setMaxDepth(quadTree.getMaxDepth()+1);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
                {
                    showVelocity = (showVelocity) ? false : true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
                {
                    showQuadTree = (showQuadTree) ? false : true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
                {
                    showParticles = (showParticles) ? false : true;
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
                    particles.emplace_back(Particle(initial_mousePosF, (initial_mousePosF-final_mousePosF), particleMass));
                }

                if (isMiddleButtonPressed && !sf::Mouse::isButtonPressed(sf::Mouse::Middle))
                {
                    isMiddleButtonPressed = false;
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

                if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
                {
                    isMiddleButtonPressed = true;
                    scroll_mousePosF = getMousePosition(*gameWindow);
                }

                break;
            
            case sf::Event::MouseWheelScrolled:	// Scroll in

                scrollDelta = event.mouseWheelScroll.delta;

                if (scrollDelta < 0) gameView.move((getMousePosition(*gameWindow) - gameView.getCenter()) * 0.13f);

                gameView.zoom(1 + (event.mouseWheelScroll.delta*0.05f));
                gameWindow->setView(gameView);

                break;
            
            default:
                break;
        }
    }
}

#define P_RADIUS_DIV_2 (0.5f / 2.0f)
#define TRI_X_OFFSET ((0.5f * std::sqrt(3.0f) / 2.0f)) 

DEFINE_API_PROFILER(PopAndSwap);
DEFINE_API_PROFILER(InsertIntoQuadTree);
DEFINE_API_PROFILER(UpdateForces);
DEFINE_API_PROFILER(DrawParticles);
DEFINE_API_PROFILER(DrawVelocities);
DEFINE_API_PROFILER(DrawQuadTree);

void ParticleSimulation::updateAndDraw()
{
    gameWindow->clear();

    quadTree.deleteTree();

    leafNodes.clear();

    if (isRightButtonPressed || isAiming) {
        current_mousePosF = getMousePosition(*gameWindow);
    }

    if (isMiddleButtonPressed) {
        gameView.move((scroll_mousePosF - getMousePosition(*gameWindow)) * 0.07f);
        gameWindow->setView(gameView);
    }

    {
        API_PROFILER(PopAndSwap);
        for (std::size_t i = 0; i < particles.size(); ++i) {

            if (particles[i].position.x < 0 || particles[i].position.x > simulationWidth ||
                particles[i].position.y > simulationHeight || particles[i].position.y < 0) {

                std::swap(particles[i], particles.back());
                particles.pop_back();
                --i;
            }
        }
    }

    globalCOM.x = 0;
    globalCOM.y = 0;

    {
        API_PROFILER(InsertIntoQuadTree);
        quadTree.insert(particles);
    }
    
    int totalLeafNodes = 0;
    globalCOM = quadTree.getLeafNodes(leafNodes, totalLeafNodes);

    if (!isPaused) {

        if (!particles.empty()) {
            API_PROFILER(UpdateForces);
            updateForces();
        }

    }

    if (!particles.empty() && showParticles) {

        {
            API_PROFILER(DrawParticles);
            
            sf::VertexArray particleVertices(sf::Triangles, particles.size() * 3);
            int vi = 0;

            for (std::size_t i = 0; i < particles.size(); ++i) {
                const float center_x = particles[i].position.x;
                const float center_y = particles[i].position.y;
                const float y_pos = center_y - P_RADIUS_DIV_2;

                // Right now to lower time for drawing function we are only drawing a triangle
                // where the particle circle would be inscribed within the triangle. This will lead total
                // some visual overlap close to the triangle vertices when particles are not actually overlapping,
                // but allows us to use a vertex array of triangles with only 3 vertices per particle for a batch render
                
                // Top vertex
                particleVertices[vi].position.x = center_x;
                particleVertices[vi].position.y = center_y + 0.5f;
                particleVertices[vi++].color = particles[i].color;

                // Left vertex
                particleVertices[vi].position.x = center_x - TRI_X_OFFSET;
                particleVertices[vi].position.y = y_pos;
                particleVertices[vi++].color = particles[i].color;

                // Right vertex
                particleVertices[vi].position.x = center_x + TRI_X_OFFSET;
                particleVertices[vi].position.y = y_pos;
                particleVertices[vi++].color = particles[i].color;
               
            }

            gameWindow->draw(particleVertices);
        }

        if (showVelocity) {
            API_PROFILER(DrawVelocities);
            drawParticleVelocity();
        }

    }

    if (isAiming) {
        drawAimLine();
    }

    if (showQuadTree) {
        API_PROFILER(DrawQuadTree);
        quadTree.display(gameWindow, totalLeafNodes);

        if (leafNodes.size() != 0) {
            sf::CircleShape circle(20.0f);
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            circle.setPosition(globalCOM);
            circle.setFillColor(sf::Color(255,0,0,20));
            gameWindow->draw(circle);
        }
    }

    particleCountText.setString("Particle count: " + std::to_string(particles.size()));
    particleMassText.setString("Particle mass: " + std::to_string( int(particleMass) ));
    
    gameWindow->draw(particleCountText);
    gameWindow->draw(particleMassText);

    if (isPaused) {
        gameWindow->draw(isPausedText);
    }
    gameWindow->display();
}

inline void ParticleSimulation::drawAimLine() 
{	
    velocityText.setPosition(initial_mousePosF.x+5.0f, initial_mousePosF.y);
    float angle =  (initial_mousePosF.y - current_mousePosF.y) / (initial_mousePosF.x - current_mousePosF.x);
    velocityText.setString(std::to_string( abs( ( atan(angle) * 180.0f ) / 3.14159f) ));

    sf::VertexArray line(sf::Lines, 2);
    line[0].position = initial_mousePosF;
    line[1].position = current_mousePosF;
    line[0].color  = sf::Color(0, 255, 0, 155);
    line[1].color = sf::Color(0, 255, 0, 25);
        
    gameWindow->draw(velocityText);
    gameWindow->draw(line);
}

inline void ParticleSimulation::drawParticleVelocity() 
{
    sf::VertexArray lines(sf::Lines, particles.size()*2);
    
    int pIdx = 0;
    for (std::size_t i = 0; i < particles.size()*2; i+=2) {
        
        lines[i+1].position.x = (particles[pIdx].position.x + particles[pIdx].velocity.x/450);
        lines[i+1].position.y = (particles[pIdx].position.y + particles[pIdx].velocity.y/450);
        lines[i].position = particles[pIdx].position;
        lines[i].color  = sf::Color(0,0,255,85);
        lines[i+1].color = sf::Color(255,0,0,0);

        pIdx++;

    }
   
    gameWindow->draw(lines);
}

static inline void attractParticleToMousePos(Particle& particle, sf::Vector2f& current_mousePosF) 
{
    particle.velocity -= sf::Vector2f(0.35f * (particle.position.x - current_mousePosF.x),
                                0.35f * (particle.position.y - current_mousePosF.y));

    //particle.acceleration -= sf::Vector2f(150.0f * (particle.position.x - current_mousePosF.x),
    //                            150.0f * (particle.position.y - current_mousePosF.y));
}

static inline void attractParticleToMousePos(Particle* particle, sf::Vector2f& current_mousePosF) 
{
    particle->velocity -= sf::Vector2f(0.35f * (particle->position.x - current_mousePosF.x),
                                0.35f * (particle->position.y - current_mousePosF.y));

    //particle->acceleration -= sf::Vector2f(150.0f * (particle->position.x - current_mousePosF.x),
    //                            150.0f * (particle->position.y - current_mousePosF.y));
}

void ParticleSimulation::updateForces()
{
    int n_threads = numThreads;
    
    if  (leafNodes.size() < static_cast<std::size_t>(numThreads)) {
        numThreads = leafNodes.size();
    }

    const std::size_t chunkSize = leafNodes.size() / numThreads;
    const std::size_t remainder = leafNodes.size() % numThreads;


    for (int i = 0; i < numThreads; i++) {
        const std::size_t startIdx = i * chunkSize;
        const std::size_t endIdx = (i==numThreads-1) ? startIdx + chunkSize + remainder : startIdx + chunkSize;
        

        auto threadFunc = [this, startIdx, endIdx]() {

            const std::vector<QuadTree::ParticleElementNode>& particleElementNodeVec = quadTree.getParticleElementNodeVec();

            for (std::size_t j = startIdx; j < endIdx; j++) {

                const int first_element_idx = leafNodes[j]->firstElement;

                for (int i = first_element_idx; i != -1; i = particleElementNodeVec[i].next_particle) {

                    int particle_index = particleElementNodeVec[i].particle_index;
                    Particle& particle = particles[particle_index];

                    for (int j = first_element_idx; j != -1; j = particleElementNodeVec[j].next_particle) {
                        int other_index = particleElementNodeVec[j].particle_index;
                        Particle& other = particles[other_index];

                        if (&other == &particle) {
                            continue;
                        }

                        const float distanceSquared = dot(particle.position - other.position,
                                                    particle.position - other.position);

                        if (distanceSquared < 0.01f) {
                            continue;
                        }
                        
                        const float radiusSquared = 1.0f;

                        const bool is_colliding = (distanceSquared <= radiusSquared);
                        
                        if (is_colliding) {
                            sf::Vector2f rHat = (other.position - particle.position) * inv_Sqrt(distanceSquared);

                            const float a1 = dot(particle.velocity, rHat);
                            const float a2 = dot(other.velocity, rHat);
                        
                            const float p = 2.0f * particle.mass * other.mass * (a1-a2)/(particle.mass + other.mass);
                            
                            particle.velocity -= p/particle.mass * (rHat);
                            other.velocity += p/other.mass * (rHat);

                        } else {

                            particle.acceleration += (other.mass / distanceSquared) * 
                                                        BIG_G * (other.position - particle.position);
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

    threads.clear();

    for (int i = 0; i < numThreads; i++) {
        const std::size_t startIdx = i * chunkSize;
        const std::size_t endIdx = (i==numThreads-1) ? startIdx + chunkSize + remainder : startIdx + chunkSize;
        
        auto threadFunc = [this, startIdx, endIdx]() {
            sf::Color c;
            const std::vector<QuadTree::ParticleElementNode>& particleElementNodeVec = quadTree.getParticleElementNodeVec();

            for (std::size_t j = startIdx; j < endIdx; j++) {

                sf::Vector2f newCOM(0,0);

                int localParticleCount = leafNodes[j]->count;
                int nonLocalParticles = (particles.size() - localParticleCount);

                if (nonLocalParticles != 0) {

                    newCOM.x = static_cast<float>(particles.size() * globalCOM.x - quadTree.getCOM(leafNodes[j]).x) /
                                    static_cast<float>(particles.size() - localParticleCount);
                    newCOM.y = static_cast<float>(particles.size() * globalCOM.y - quadTree.getCOM(leafNodes[j]).y) /
                                    static_cast<float>(particles.size() - localParticleCount);
                }

                for (int i = leafNodes[j]->firstElement; i != -1; i = particleElementNodeVec[i].next_particle) {

                    int particle_index = particleElementNodeVec[i].particle_index;
                    Particle& particle = particles[particle_index];

                    if (nonLocalParticles != 0) {
                        const float distanceSquared = dot(particle.position - newCOM,
                                                      particle.position - newCOM);

                        particle.acceleration += (nonLocalParticles / distanceSquared) * BIG_G *
                                                        (newCOM - particle.position);
                    }

                    if (isRightButtonPressed) {
                        attractParticleToMousePos(particle, current_mousePosF);
                    }

                    particle.velocity += particle.acceleration * timeStep;
                    particle.position += particle.velocity * timeStep;
                    
                    float vel = std::sqrt(particle.velocity.x * particle.velocity.x +
                                particle.velocity.y * particle.velocity.y);


                    float maxVel = 3000.0f;

                    if (vel > maxVel) vel = maxVel;
                    
                    float p = vel / maxVel;

                    c.r = static_cast<uint8_t>(15.0f + (240.0f * p));
                    c.g = 0;
                    c.b = static_cast<uint8_t>(240.0f * (1.0f-p));
                    c.a = static_cast<uint8_t>(30.0f + (225.0f * p));

                    particle.color = c;

                    particle.acceleration.x = 0.0f;
                    particle.acceleration.y = 0.0f;

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

void ParticleSimulation::addSierpinskiTriangleParticleChunk(const int x, const int y, const int size, const int depth)
{
    if (depth == 0) {
        particles.emplace_back(Particle(sf::Vector2f(x,y), sf::Vector2f(0,0), particleMass));
    } else {
        const int halfSize = size/2;

        addSierpinskiTriangleParticleChunk(x, y, halfSize, depth - 1);
        addSierpinskiTriangleParticleChunk(x + halfSize, y, halfSize, depth - 1);
        addSierpinskiTriangleParticleChunk(x + halfSize / 2, y + halfSize, halfSize, depth - 1);

    }
}

void ParticleSimulation::addCheckeredParticleChunk()
{
    for (int i = simulationWidth/3; i < ((2*simulationWidth)/3); ++i) {
        for (int j = simulationHeight/3; j < ((2*simulationHeight)/3); ++j) {
            if((i/7) % 6 == (j/5) % 6)
                particles.emplace_back(Particle(sf::Vector2f(i,j), sf::Vector2f(0,0), particleMass));
        }
    }
}

void ParticleSimulation::addParticleDiagonal(int tiles, int particleNum)
{
    const int col = sqrt(particleNum/tiles);
    const int row = col;

    const float smallWidth = static_cast<float>(simulationWidth) / tiles;
    const float smallHeight = static_cast<float>(simulationHeight) / tiles;

    for (int i = 0; i < tiles; i++) {
        for (int j = 0; j < col; j++) {
            for (int k = 0; k < row; k++ ) {
                const float x = (j * smallWidth / col) + smallWidth * i;
                const float y = (k * smallHeight / row) + smallHeight * i;
                particles.emplace_back(Particle(sf::Vector2f(x,y), sf::Vector2f(0,0), particleMass));
            }
        }
    }
}

void ParticleSimulation::addParticleDiagonal2(int tiles, int particleNum)
{
    const int col = sqrt(particleNum / tiles);
    const int row = col;

    const float smallWidth = static_cast<float>(simulationWidth) / tiles;
    const float smallHeight = static_cast<float>(simulationHeight) / tiles;

    for (int i = 0; i < tiles; i++) {
        for (int j = 0; j < col; j++) {
            for (int k = 0; k < row; k++) {
                const float x = static_cast<float>(simulationWidth) - ((j * smallWidth / col) + smallWidth * i);
                const float y = (k * smallHeight / row) + smallHeight * i;

                particles.emplace_back(Particle(sf::Vector2f(x, y), sf::Vector2f(0, 0), particleMass));
            }
        }
    }
}

