#include "QuadTree.hpp"

QuadTree::QuadTree()
  : m_level(0),
    treeMaxDepth(7),
    nodeCap(4),
    width(1280),
    height(800),
    isLeaf(true),
    m_subnode{nullptr, nullptr, nullptr, nullptr},
    particleMutex()
{

    m_index.reserve(nodeCap + 1);
    m_rect.setPosition(1280-width, 800-height);
    m_rect.setSize(sf::Vector2f(width,height));
    m_rect.setOutlineColor(sf::Color(0,255,0,45));
    m_rect.setOutlineThickness(1);
    m_rect.setFillColor(sf::Color::Transparent);
    totalMass = 0;
    com = sf::Vector2f(0,0);
}

// This constructor is used for all subnodes and takes in a position.
// The origin of each region is the top left corner, and should be the position passed in.
QuadTree::QuadTree(const int m_level, sf::Vector2f position, float w, float h, int maxDepth, int capacity)
  : m_level(m_level),
    treeMaxDepth(maxDepth),
    nodeCap(capacity),
    width(w),
    height(h),
    isLeaf(true),
    m_subnode{nullptr, nullptr, nullptr, nullptr},
    particleMutex()
{
    m_index.reserve(nodeCap + 1);
    m_rect.setPosition(position);
    m_rect.setSize(sf::Vector2f(width,height));
    m_rect.setOutlineColor(sf::Color(0,255,0,45));
    m_rect.setOutlineThickness(1);
    m_rect.setFillColor(sf::Color::Transparent);
    totalMass = 0;
    com = sf::Vector2f(0,0);
}

//This constructor is used for the root node and does not take in a starting position
QuadTree::QuadTree(const int m_level, float w, float h, int maxDepth, int capacity)
  : m_level(m_level),
    treeMaxDepth(maxDepth),
    nodeCap(capacity),
    width(w),
    height(h),
    isLeaf(true),
    m_subnode{nullptr, nullptr, nullptr, nullptr},
    particleMutex()
{
    m_index.reserve(nodeCap + 1);
    m_rect.setPosition(0,0);
    m_rect.setSize(sf::Vector2f(width,height));
    m_rect.setOutlineColor(sf::Color(0,255,0,45));
    m_rect.setOutlineThickness(1);
    m_rect.setFillColor(sf::Color::Transparent);
    totalMass = 0;
    com = sf::Vector2f(0,0);
}

QuadTree::QuadTree(const QuadTree& qt)
  : m_level(qt.m_level),
    treeMaxDepth(qt.treeMaxDepth),
    nodeCap(qt.nodeCap),
    width(qt.width),
    height(qt.height),
    isLeaf(qt.isLeaf),
    m_subnode{qt.m_subnode},
    m_index{qt.m_index},
    particleMutex()
{
    m_rect.setPosition(qt.m_rect.getPosition());
    m_rect.setSize(sf::Vector2f(width,height));
    m_rect.setOutlineColor(sf::Color(0,255,0,45));
    m_rect.setOutlineThickness(1);
    m_rect.setFillColor(sf::Color::Transparent);
    totalMass = qt.totalMass;
    com = qt.com;
}

QuadTree::QuadTree(QuadTree&& qt)
  : m_level(qt.m_level),
    treeMaxDepth(qt.treeMaxDepth),
    nodeCap(qt.nodeCap),
    width(qt.width),
    height(qt.height),
    isLeaf(qt.isLeaf),
    m_subnode(std::move(qt.m_subnode)),
    m_index(std::move(qt.m_index)),
    m_rect(std::move(qt.m_rect)),
    com(qt.com),
    totalMass(qt.totalMass),
    particleMutex()
{
    qt.com = sf::Vector2f(0, 0);
    qt.totalMass = 0;
}

QuadTree& QuadTree::operator=(const QuadTree& other)
{
    if (this != &other) {
        m_level = other.m_level;
        treeMaxDepth = other.treeMaxDepth;
        nodeCap = other.nodeCap;
        width = other.width;
        height = other.height;
        isLeaf = other.isLeaf;
        m_subnode = other.m_subnode;
        m_index = other.m_index;
        m_rect = other.m_rect;
        com = other.com;
        totalMass = other.totalMass;
    }
    return *this;
}

QuadTree& QuadTree::operator=(QuadTree&& other)
{
    if (this != &other) {
        m_level = other.m_level;
        treeMaxDepth = other.treeMaxDepth;
        nodeCap = other.nodeCap;
        width = other.width;
        height = other.height;
        isLeaf = other.isLeaf;
        m_subnode = std::move(other.m_subnode);
        m_index = std::move(other.m_index);
        m_rect = std::move(other.m_rect);
        com = other.com;
        totalMass = other.totalMass;
        other.com = sf::Vector2f(0, 0);
        other.totalMass = 0;
    }
    return *this;
}


void QuadTree::split()
{
    isLeaf = false;

    // Use current origin position of bounding rectangle to calculate origin position for NW,NE,SW,SE subregions
    m_subnode[0] = new QuadTree(m_level + 1, sf::Vector2f(m_rect.getPosition().x, m_rect.getPosition().y),
                width/2, height/2, treeMaxDepth, nodeCap);
    
    m_subnode[1] = new QuadTree(m_level + 1, sf::Vector2f(m_rect.getPosition().x + width/2,
			    m_rect.getPosition().y), width/2, height/2, treeMaxDepth, nodeCap);
    
    m_subnode[2] = new QuadTree(m_level + 1, sf::Vector2f(m_rect.getPosition().x,
			    m_rect.getPosition().y + height/2), width/2, height/2, treeMaxDepth, nodeCap);
    
    m_subnode[3] = new QuadTree(m_level + 1, sf::Vector2f(m_rect.getPosition().x + width/2,
			    m_rect.getPosition().y + height/2), width/2, height/2, treeMaxDepth, nodeCap);

    for (Particle* particle : m_index)
    {
        for (QuadTree* subNode : m_subnode)
        {
            if (subNode->m_rect.getGlobalBounds().contains(particle->position))
            {
                subNode->insert(particle);
                break;
            }
        }
    }

    m_index.clear();
    totalMass = 0;
    com.x = 0;
    com.y = 0;
}

void QuadTree::display(sf::RenderWindow* gameWindow)
{
    if (isLeaf)
    {
        gameWindow->draw(m_rect);
        return;
    }

    m_subnode[0]->display(gameWindow);
    m_subnode[1]->display(gameWindow);
    m_subnode[2]->display(gameWindow);
    m_subnode[3]->display(gameWindow);
}

void QuadTree::insert(Particle* particle)
{
    //If Quadtree node is a leaf node, insert and split if node greater than capacity
    if (isLeaf)
    {
        m_index.push_back(particle);

        if (m_index.size() > nodeCap && m_level < treeMaxDepth)
        {
            split();
        }
        else
        {
            totalMass += particle->mass;
            com.x += particle->position.x * particle->mass;
            com.y += particle->position.y * particle->mass;
        }
      
        return;
    }

    // If not leaf check subnode which particle is contained in and insert
    for (QuadTree* subNode : m_subnode)
    {
        if (subNode->m_rect.getGlobalBounds().contains(particle->position))
        {
            subNode->insert(particle);
            break;
        }
    }
}

void QuadTree::updateForces(float dt, Particle* particle)
{
    if (!isLeaf)
    {
        m_subnode[0]->updateForces(dt, particle);
        m_subnode[1]->updateForces(dt, particle);
        m_subnode[2]->updateForces(dt, particle);
        m_subnode[3]->updateForces(dt, particle);
    }
    else if (m_rect.getGlobalBounds().contains(particle->position))
    {
        for (Particle* other : m_index)
        {
            if (other == particle)
            {
	            continue;
            }
        
            float distanceSquared = dot(particle->position - other->position, particle->position - other->position);
            
            if (distanceSquared != 0 && distanceSquared > (particle->radius + other->radius) * (particle->radius + other->radius))
            {
                particle->acceleration += (other->mass / distanceSquared) * BIG_G * (other->position - particle->position);
            }

            if (distanceSquared != 0 && distanceSquared <= (particle->radius + other->radius) * (particle->radius + other->radius))
            {
                std::lock_guard<std::mutex> lock(particleMutex);

                sf::Vector2f rHat = (other->position - particle->position) * inv_Sqrt(distanceSquared);

                float a1 = dot(particle->velocity, rHat);
                float a2 = dot(other->velocity, rHat);
              
                float p = 2 * particle->mass * other->mass * (a1-a2)/(particle->mass + other->mass);

                particle->velocity -= p/particle->mass * (rHat);
                other->velocity += p/other->mass * (rHat);
            }
        }    
    }
    else if (!m_index.empty())
    {
        float x = com.x / totalMass;
        float y = com.y / totalMass;
        float distanceSquared = dot(particle->position - sf::Vector2f(x,y), particle->position - sf::Vector2f(x,y));
        particle->acceleration += (totalMass / distanceSquared) * BIG_G * (sf::Vector2f(x,y) - particle->position);
    }

    // // This calculates the new position and velocity after summing all the forces on the particle
    // if (m_level == 0)
    // {
    //     std::lock_guard<std::mutex> lock(particleMutex);
    //     particle->velocity += particle->acceleration * dt;
    //     particle->position += particle->velocity * dt;
	//     particle->acceleration.x = 0.0f;
	//     particle->acceleration.y = 0.0f;
    // }
}

void QuadTree::deleteTree()
{
    // If we are at the base node and it is the leaf clear Particle vector and return;
    if (m_level == 0 && isLeaf)
    {
        com.x = 0.0f;
	    com.y = 0.0f;
        totalMass = 0;
        m_index.clear();
        return;
    }

    // Loop through subNodes for current node of QuadTree
    for (QuadTree* subNode : m_subnode)
    {
        if (subNode == nullptr) // If one is nullptr all are nullptr so break
        {
            break;
        }
        else if (subNode->isLeaf) // If subnode is a leaf delete
        {
            delete subNode;
            subNode = nullptr;
        }
        else // If not a leaf recursively call down tree, and delete after function returns
        {
            subNode->deleteTree();
            delete subNode;
            subNode = nullptr;
        }
    }

    // Root node is not deleted; just clear Particle vector
    if ( m_level == 0 )
    {
        com.x = 0;
	    com.y = 0;
        totalMass = 0;
        m_index.clear();
        isLeaf = true;
    }
}

void QuadTree::getLeafNodes(std::vector<QuadTree*>& vec)
{
    if (isLeaf)
    {
        vec.push_back(this);
        return;
    }

    m_subnode[0]->getLeafNodes(vec);
    m_subnode[1]->getLeafNodes(vec);
    m_subnode[2]->getLeafNodes(vec);
    m_subnode[3]->getLeafNodes(vec);
}

bool QuadTree::contains(sf::Vector2f& pos)
{
    if (m_rect.getGlobalBounds().contains(pos))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool QuadTree::empty()
{
    if (m_index.empty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::mutex& QuadTree::getParticleMutex()
{
    return particleMutex;
}

std::vector<Particle*>& QuadTree::getParticleVec()
{
    return m_index;
}

sf::Vector2f& QuadTree::getCOM()
{
    return com;
}

int& QuadTree::getTotalMass()
{
    return totalMass;
}

int QuadTree::getMaxDepth()
{
    return nodeCap;
}

int QuadTree::getNodeCap()
{
    return treeMaxDepth;
}

template <typename T>
inline float dot(const sf::Vector2<T>& vec1, const sf::Vector2<T>& vec2)
{
    return (vec1.x * vec2.x) + (vec1.y * vec2.y);
}

inline float inv_Sqrt(float number)
{
    float squareRoot = sqrt(number);
    return 1.0f / squareRoot;
}
