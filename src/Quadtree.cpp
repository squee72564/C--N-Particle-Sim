#include "QuadTree.hpp"

inline QuadTree::Node createNode(const int m_level, const int w, const int h, const sf::Vector2f ori) {
    QuadTree::Node ret;
    ret.m_level = m_level;
    ret.m_rect = sf::RectangleShape(sf::Vector2f(w,h));

    ret.m_rect.setFillColor(sf::Color::Transparent);
    ret.m_rect.setOutlineColor(sf::Color(0,255,0,100));
    ret.m_rect.setOutlineThickness(1.0f);
    ret.m_rect.setPosition(ori);

    ret.isLeaf = true;
    ret.com.x = 0;
    ret.com.y = 0;
    ret.totalMass = 0;
    ret.m_index.reserve(24);

    return ret;
}

void QuadTree::initTree()
{
    const int totalNodes = pow(4,treeMaxDepth+1)-1;
    nodes.resize(totalNodes);

    std::stack<int> stack;

    // Start with root
    stack.push(0);

    nodes[0] = createNode(
                    0,
                    1920,
                    1080,
                    sf::Vector2f(0,0));
    
    while (!stack.empty()) {
        const int currIdx = stack.top();
        stack.pop();

        if (currIdx >= totalNodes)
        {
            //std::cout << "Continuing at at max node: " << currIdx << "/" << totalNodes << "\n";
            continue;
        }

        assert(currIdx < static_cast<int>(nodes.capacity()));

        const QuadTree::Node& currentNode = nodes[currIdx];

        const int currDepth = currentNode.m_level;

        if (currDepth >= treeMaxDepth)
        {
            //std::cout << "Continuing at setup for max depth: " << currDepth << "/" << treeMaxDepth << "\n";
            continue;
        }

        const int w = currentNode.m_rect.getSize().x;
        const int h = currentNode.m_rect.getSize().y;
        const sf::Vector2f currPos = currentNode.m_rect.getPosition();

        const sf::Vector2f childPositions[4]= {
            currPos,
            sf::Vector2f(currPos.x + w/2, currPos.y), 
            sf::Vector2f(currPos.x, currPos.y + h/2), 
            sf::Vector2f(currPos.x + w/2, currPos.y + h/2), 
        };
        
        for (int i = 1; i <= 4; i++) {
            const int child_idx = 4 * currIdx + i;

            nodes[child_idx] = createNode(
                                    currDepth+1,
                                    w/2,
                                    h/2,
                                    childPositions[i-1]);

            stack.push(child_idx);
        }

    }
}

QuadTree::QuadTree()
  : w(1920),
    h(1080),
    treeMaxDepth(7),
    nodeCap(4)
{
    std::cout << "Default Constructor for QuadTree called.\n";
}

QuadTree::QuadTree(const int w, const int h, const int maxDepth, const int capacity)
  : w(w),
    h(h),
    treeMaxDepth(maxDepth),
    nodeCap(capacity)
{
    std::cout << "Non-default Constructor for QuadTree called.\n";
    // Initially set up vector for nodes, this will depend on max depth
    const int totalPossibleNodes = pow(4,treeMaxDepth+1)-1;
    nodes.reserve(totalPossibleNodes);
    std::cout << "Nodes reserved.\nSetting up quadtree...\n";
    initTree();
    std::cout << "Tree initialized.\n";
}

QuadTree::QuadTree(const QuadTree& other)
{
    std::cout << "Copy Constructor called\n";
    w = other.w;
    h = other.h;
    treeMaxDepth = other.treeMaxDepth;
    nodeCap = other.nodeCap;
    nodes = other.nodes;
}

QuadTree::QuadTree(QuadTree&& other) noexcept
{
    std::cout << "Move Constructor called\n";
    w = std::exchange(other.w, 1920);
    h = std::exchange(other.h, 1080);
    treeMaxDepth = std::exchange(other.treeMaxDepth, 0);
    nodeCap = std::exchange(other.nodeCap, 0);
    nodes = std::move(other.nodes);
}

QuadTree& QuadTree::operator=(const QuadTree& other)
{
    std::cout << "Copy assignment called\n";
    if (this != &other) {
        w = other.w;
        h = other.h;
        treeMaxDepth = other.treeMaxDepth;
        nodeCap = other.nodeCap;
        nodes = other.nodes;
    }

    return *this;
}
 
QuadTree& QuadTree::operator=(QuadTree&& other) noexcept
{
    std::cout << "Move assignment called\n";
    if (this != &other) {
        w = std::exchange(other.w, 1920);
        h = std::exchange(other.h, 1080);
        treeMaxDepth = std::exchange(other.treeMaxDepth, 0);
        nodeCap = std::exchange(other.nodeCap, 0);
        nodes = std::move(other.nodes);
    }

    return *this;
}

QuadTree::~QuadTree()
{
    std::cout << "Destructor called\n";
    nodes.clear();
}


void QuadTree::display(sf::RenderWindow* gameWindow)
{
    std::stack<int> stack;

    // Start with root
    stack.push(0);
    
    while (!stack.empty()) {
        const int currIdx = stack.top();
        const QuadTree::Node currentNode = nodes[currIdx];
        stack.pop();

        if (currentNode.isLeaf) {
            //std::cout << "at idx " << currIdx << " with rect at pos ("
            //    << currentNode.m_rect.getPosition().x << ","
            //    << currentNode.m_rect.getPosition().y << ")"
            //    << " and width " << currentNode.m_rect.getSize().x
            //    << " and height " << currentNode.m_rect.getSize().y << "\n";

            gameWindow->draw(currentNode.m_rect);
        } else {
            for (int i = 1; i <= 4; i++) {
                const int child_idx = 4 * currIdx + i;
                stack.push(child_idx);
            }
        }
    }
}

void QuadTree::insert(Particle* particle, int rootIdx)
{
    std::stack<int> stack;

    stack.push(rootIdx);

    
    while (!stack.empty()) {
        const int currIdx = stack.top();
        stack.pop();

        QuadTree::Node* currNode = &nodes[currIdx];
        const int currDepth = currNode->m_level;

        if (currNode->isLeaf) {
            
            currNode->m_index.emplace_back(particle);
            const size_t particleNum = currNode->m_index.size();

            if (particleNum > nodeCap && currDepth < treeMaxDepth) {
                //std::cout << "SPLITTING\n";
                split(currIdx);
            } else {
                //std::cout << "ADDING particle: " << particleNum << "/" << nodeCap << "\n";
                //std::cout << "\tAt depth: " << currDepth << "/" << treeMaxDepth << "\n";
                currNode->totalMass += particle->mass;
                currNode->com.x += particle->position.x * particle->mass;
                currNode->com.y += particle->position.y * particle->mass;
                //std::cout << "\tTotalMass: " << currNode->totalMass << "\n";
            }

            continue;
        }

        
        for (int i = 1; i <= 4; i++) {
            const int child_idx = 4 * currIdx + i;

            if (contains(&nodes[child_idx], particle->position)) {
                stack.push(child_idx);
                break;
            }
        }
    }
}

void QuadTree::split(const int parentIdx)
{
    QuadTree::Node& parentNode = nodes[parentIdx];
    parentNode.isLeaf = false;

    for (Particle* particle : parentNode.m_index) {
        for (int i = 1; i <= 4; i++) {
            const int child_idx = 4 * parentIdx + i;
            if (contains(&nodes[child_idx], particle->position)) {
                insert(particle, child_idx);
                break;
            }
        }
    }

    parentNode.m_index.clear();
    parentNode.totalMass = 0;
    parentNode.com.x = 0;
    parentNode.com.y = 0;
}

void QuadTree::deleteTree()
{
    std::stack<int> stack;

    // Start with root
    stack.push(0);
    
    while (!stack.empty()) {
        const int currIdx = stack.top();
        QuadTree::Node& currentNode = nodes[currIdx];
        stack.pop();

        if (nodes[currIdx].isLeaf) {
            if (!currentNode.m_index.empty()) currentNode.m_index.clear();
        } else {
            for (int i = 1; i <= 4; i++) {
                stack.push(4 * currIdx + i);
            }
        }

        currentNode.com.x = 0;
        currentNode.com.y = 0;
        currentNode.totalMass = 0;
        currentNode.isLeaf = true;
    }
}

void QuadTree::getLeafNodes(std::vector<QuadTree::Node*>& vec)
{
    std::stack<int> stack;

    // Start with root
    stack.push(0);
    
    while (!stack.empty()) {
        const int currIdx = stack.top();
        QuadTree::Node* currentNode = &nodes[currIdx];
        stack.pop();

        if (currentNode->isLeaf) {
            //std::cout << "at idx " << currIdx << " with rect at pos ("
            //    << currentNode->m_rect.getPosition().x << ","
            //    << currentNode->m_rect.getPosition().y << ")"
            //    << " and width " << currentNode->m_rect.getSize().x
            //    << " and height " << currentNode->m_rect.getSize().y << "\n";
            vec.push_back(currentNode);
        } else {
            for (int i = 1; i <= 4; i++) {
                const int child_idx = 4 * currIdx + i;
                stack.push(child_idx);
            }
        }
    }
}

bool QuadTree::contains(const QuadTree::Node* node, const sf::Vector2f& pos)
{
    return node->m_rect.getGlobalBounds().contains(pos);
}

bool QuadTree::empty(const QuadTree::Node* node)
{
    return node->m_index.empty();
}

const std::vector<Particle*>& QuadTree::getParticleVec(const QuadTree::Node* node)
{
    return node->m_index;
}

const sf::Vector2f& QuadTree::getCOM(const QuadTree::Node* node)
{
    return node->com;
}

int QuadTree::getTotalMass(const QuadTree::Node* node)
{
    return node->totalMass;
}

int QuadTree::getMaxDepth()
{
    return nodeCap;
}

int QuadTree::getNodeCap()
{
    return treeMaxDepth;
}

