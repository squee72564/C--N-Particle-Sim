#include "QuadTree.hpp"

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

static inline QuadTree::Node createNode(const int m_level, const float w, const float h, const sf::Vector2f ori) {
    QuadTree::Node ret;
    ret.m_level = m_level;
    ret.m_rect = sf::RectangleShape(sf::Vector2f(w,h));

    ret.m_rect.setFillColor(sf::Color::Transparent);
    ret.m_rect.setOutlineColor(sf::Color(0,255,0,45));
    ret.m_rect.setOutlineThickness(1.0f);
    ret.m_rect.setPosition(ori);

    ret.isLeaf = true;
    ret.com.x = 0;
    ret.com.y = 0;
    ret.totalMass = 0;
    ret.m_index.reserve(128);

    return ret;
}

void QuadTree::initTree()
{
    const int totalNodes = pow(4,treeMaxDepth+1)-1;
    nodes.resize(totalNodes);

    int array[(treeMaxDepth * 4) + 1];

    int top = 0;
    array[top++] = 0;
    
    nodes[0] = createNode(
                    0,
                    1920,
                    1080,
                    sf::Vector2f(0,0));
    
    while (top > 0) {
        const int currIdx = array[--top];

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

        const float w = currentNode.m_rect.getSize().x/2.0f;
        const float h = currentNode.m_rect.getSize().y/2.0f;
        const sf::Vector2f currPos = currentNode.m_rect.getPosition();

        const sf::Vector2f childPositions[4]= {
            currPos,
            sf::Vector2f(currPos.x + w, currPos.y), 
            sf::Vector2f(currPos.x, currPos.y + h), 
            sf::Vector2f(currPos.x + w, currPos.y + h), 
        };
        
        for (int i = 1; i <= 4; i++) {
            const int child_idx = 4 * currIdx + i;

            nodes[child_idx] = createNode(
                                    currDepth+1,
                                    w,
                                    h,
                                    childPositions[i-1]);

            array[top++] = child_idx;
        }

    }
}

struct NodeData {
    int depth;
    sf::Vector2f p;
    sf::Vector2f s;
};


void QuadTree::display(sf::RenderWindow* gameWindow, int totalLeafNodes)
{
    const int n = (totalLeafNodes * 8); 
    sf::VertexArray lines(sf::Lines, n); // this is how many lines we need for all grids
    long vi = 0;

    const sf::Color colors[4] = {
        sf::Color(0,0,204,12),
        sf::Color(102,0,204,12),
        sf::Color(0,102,204,12),
        sf::Color(255,0,127,12),
    };

    std::pair<int, NodeData> array[(treeMaxDepth * 4) + 1]; // <idx, nodeInfo>

    int top = 0;
    
    NodeData node;
    node.depth = 0;
    node.p = sf::Vector2f(0.0f, 0.0f);
    node.s = sf::Vector2f(w, h);

    std::pair<int, NodeData> nodePair = std::make_pair(0,node);

    array[top++] = nodePair;
    
    while (top > 0) {
        const int currIdx = array[--top].first;
        const int currDepth = array[top].second.depth;
        const sf::Vector2f currP = array[top].second.p;
        const sf::Vector2f currS = array[top].second.s;

        const QuadTree::Node currentNode = nodes[currIdx];


        if (currentNode.isLeaf) {

            const sf::Vector2f positions[4] = {
                currP,
                sf::Vector2f(currP.x + currS.x, currP.y),
                sf::Vector2f(currP.x + currS.x, currP.y + currS.y),
                sf::Vector2f(currP.x, currP.y + currS.y),
            };
            
            for (int i = 0; i < 4; ++i) {
                lines[vi].position= positions[i];
                lines[vi++].color = colors[i];

                lines[vi].position = positions[(i+1)%4];
                lines[vi++].color = colors[(i+1)%4];
            }


        } else {

            const sf::Vector2f cSize(currS.x / 2.0f, currS.y / 2.0f);
            const sf::Vector2f offsets[4] = {
                currP,
                sf::Vector2f(currP.x + cSize.x, currP.y), 
                sf::Vector2f(currP.x, currP.y + cSize.y), 
                sf::Vector2f(currP.x + cSize.x, currP.y + cSize.y), 
            };

            for (int i = 1; i <= 4; ++i) {
                const int child_idx = 4 * currIdx + i;

                node = {currDepth+1, offsets[i-1], cSize};

                nodePair.first = child_idx;
                nodePair.second = node;

                array[top++] = nodePair;
            }
        }
    }
    gameWindow->draw(lines);
}

void QuadTree::insert(Particle* particle, int rootIdx)
{
    int array[(treeMaxDepth * 4) + 1];

    int top = 0;
    array[top++] = rootIdx;

    while (top > 0) {
        const int currIdx = array[--top];
        QuadTree::Node& currNode = nodes[currIdx];
        const int currDepth = currNode.m_level;

        if (currNode.isLeaf) {
            currNode.m_index.emplace_back(particle);
            const size_t particleNum = currNode.m_index.size();

            if (currDepth < treeMaxDepth && particleNum > nodeCap) {
                split(currIdx);
            } else {
                currNode.totalMass += particle->mass;
                currNode.com.x += particle->position.x * particle->mass;
                currNode.com.y += particle->position.y * particle->mass;
            }

            continue;
        }

        for (int i = 1; i <= 4; i++) {
            const int child_idx = 4 * currIdx + i;

            if (contains(&nodes[child_idx], particle->position)) {
                array[top++] = child_idx;
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
    int array[(treeMaxDepth * 4) + 1];

    int top = 0;
    array[top++] = 0;

    while (top > 0) {
        const int currIdx = array[--top];
        QuadTree::Node& currentNode = nodes[currIdx];

        if (nodes[currIdx].isLeaf) {
            if (!currentNode.m_index.empty()) currentNode.m_index.clear();
        } else {
            for (int i = 1; i <= 4; i++) {
                const int child_idx = 4 * currIdx + i;
                array[top++] = child_idx;
            }
        }

        currentNode.com.x = 0;
        currentNode.com.y = 0;
        currentNode.totalMass = 0;
        currentNode.isLeaf = true;
    }
}

sf::Vector2f QuadTree::getLeafNodes(std::vector<QuadTree::Node*>& vec, int& totalLeafNodes)
{
    sf::Vector2f globalCOM(0,0);
    float _totalMass = 0;

    int array[(treeMaxDepth * 4) + 1];

    int top = 0;
    array[top++] = 0;

    // reset max depth tracking variable
    totalLeafNodes = 0;

    while (top > 0) {
        const int currIdx = array[--top];
        QuadTree::Node* currentNode = &nodes[currIdx];

        if (currentNode->isLeaf) {
            totalLeafNodes++;

            if (currentNode->m_index.empty()) {
                continue;
            }

            vec.push_back(currentNode);

            globalCOM.x += currentNode->com.x;
            globalCOM.y += currentNode->com.y;
            _totalMass += currentNode->totalMass;
        } else {
            for (int i = 1; i <= 4; i++) {
                const int child_idx = 4 * currIdx + i;
                array[top++] = child_idx;
            }
        }
    }
    
    if (_totalMass > 0.0f) {
        globalCOM.x /= _totalMass;
        globalCOM.y /= _totalMass;
    } else {
        globalCOM.x = 0.0f;
        globalCOM.y = 0.0f;
    }

    return globalCOM;
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
    return treeMaxDepth;
}

int QuadTree::getNodeCap()
{
    return nodeCap;
}

void QuadTree::setMaxDepth(int depth)
{
    treeMaxDepth = depth;
}
