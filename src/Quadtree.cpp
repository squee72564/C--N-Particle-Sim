#include <iostream>

#include "QuadTree.hpp"

static inline int calculateTotalNodes(const int depth) {
    int total = 0;
    for (int i = 0; i <= depth; ++i) {
        total += std::pow(4,i);
    }
    return total;
}

static QuadTree::Node createNode() {
    QuadTree::Node ret;

    ret.isLeaf = true;

    ret.com.x = 0;
    ret.com.y = 0;
    ret.totalMass = 0;

    ret.m_index = std::vector<Particle*>();
    ret.m_index.reserve(128);

    return ret;
}

struct NodeData {
    int depth;
    sf::Vector2f p;
    sf::Vector2f s;
};

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
    int reserved = calculateTotalNodes(maxDepth);
    quadTreeNodes = std::vector<QuadTree::Node>(reserved, createNode());
    //quadTreeNodes.reserve(reserved);
    
    initTree();
    std::cout << "Tree initialized with " << quadTreeNodes.size() << " nodes.\n";
}

QuadTree::QuadTree(const QuadTree& other)
{
    std::cout << "Copy Constructor called\n";
    w = other.w;
    h = other.h;
    treeMaxDepth = other.treeMaxDepth;
    nodeCap = other.nodeCap;
    quadTreeNodes = other.quadTreeNodes;
}

QuadTree::QuadTree(QuadTree&& other) noexcept
{
    std::cout << "Move Constructor called\n";
    w = std::exchange(other.w, 1920);
    h = std::exchange(other.h, 1080);
    treeMaxDepth = std::exchange(other.treeMaxDepth, 0);
    nodeCap = std::exchange(other.nodeCap, 0);
    quadTreeNodes = std::move(other.quadTreeNodes);
}

QuadTree& QuadTree::operator=(const QuadTree& other)
{
    std::cout << "Copy assignment called\n";
    if (this != &other) {
        w = other.w;
        h = other.h;
        treeMaxDepth = other.treeMaxDepth;
        nodeCap = other.nodeCap;
        quadTreeNodes = other.quadTreeNodes;
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
        quadTreeNodes = std::move(other.quadTreeNodes);
    }

    return *this;
}

QuadTree::~QuadTree()
{
    std::cout << "Destructor called\n";
    quadTreeNodes.clear();
}

void QuadTree::initTree()
{
    std::pair<int, NodeData> array[(treeMaxDepth * 4)]; // <idx, nodeInfo>
    int top = 0;

    NodeData node;
    node.depth = 0;
    node.p = sf::Vector2f(0,0);
    node.s = sf::Vector2f(w,h);
    
    std::pair<int, NodeData> nodePair = std::make_pair(0,node);

    array[top++] = nodePair;

    quadTreeNodes[0] = createNode();

    while (top > 0) {
        const int currIdx = array[--top].first;
        const int currDepth = array[top].second.depth;
        const sf::Vector2f currP = array[top].second.p;
        const sf::Vector2f currS = array[top].second.s;
        
        if (currDepth >= treeMaxDepth)
        {
            continue;
        }

        const sf::Vector2f cSize(currS.x / 2.0f, currS.y / 2.0f);
        const sf::Vector2f offsets[4] = {
            currP,
            sf::Vector2f(currP.x + cSize.x, currP.y), 
            sf::Vector2f(currP.x, currP.y + cSize.y), 
            sf::Vector2f(currP.x + cSize.x, currP.y + cSize.y), 
        };
        
        for (int i = 1; i <= 4; i++) {
            const int child_idx = 4 * currIdx + i;

            quadTreeNodes[child_idx] = createNode();

            node = {currDepth+1, offsets[i-1], cSize};

            nodePair.first = child_idx;
            nodePair.second = node;

            array[top++] = nodePair;
        }

    }
}


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

        const QuadTree::Node& currentNode = quadTreeNodes[currIdx];

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

void QuadTree::insert(Particle* particle)
{
    //std::pair<int, int> array[(treeMaxDepth * 4) + 1]; // <idx, depth>

    //int top = 0;
    //array[top++] = std::make_pair(0, 0);

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

        //const QuadTree::Node& currentNode = quadTreeNodes[currIdx];
        //const int currIdx = array[--top].first;
        //const int currDepth = array[top].second;
        QuadTree::Node& currNode = quadTreeNodes[currIdx];

        const sf::Vector2f cSize(currS.x / 2.0f, currS.y / 2.0f);
        const sf::Vector2f offsets[4] = {
            currP,
            sf::Vector2f(currP.x + cSize.x, currP.y), 
            sf::Vector2f(currP.x, currP.y + cSize.y), 
            sf::Vector2f(currP.x + cSize.x, currP.y + cSize.y), 
        };

        if (currNode.isLeaf) {
            const size_t particleNum = currNode.m_index.size();

            if (currDepth < treeMaxDepth && particleNum+1 > nodeCap) {
                split(currIdx, cSize, offsets);
            } else {
                currNode.m_index.emplace_back(particle);
                currNode.totalMass += particle->mass;
                currNode.com.x += particle->position.x * particle->mass;
                currNode.com.y += particle->position.y * particle->mass;
                continue;
            }
        }

        for (int i = 1; i <= 4; i++) {
            const int child_idx = 4 * currIdx + i;

            if (sf::FloatRect(offsets[i-1], cSize).contains(particle->position)) {
                node = {currDepth+1, offsets[i-1], cSize};

                nodePair.first = child_idx;
                nodePair.second = node;

                array[top++] = nodePair;

                break;
            }
        }
    }
}

void QuadTree::split(const int parentIdx, const sf::Vector2f& childSize, const sf::Vector2f* childOffsets)
{
    QuadTree::Node& parentNode = quadTreeNodes[parentIdx];

    parentNode.isLeaf = false;
    parentNode.totalMass = 0;
    parentNode.com.x = 0;
    parentNode.com.y = 0;
    
    for (Particle* particle : parentNode.m_index) {
        for (int i = 1; i <= 4; i++) {
            const int child_idx = 4 * parentIdx + i;
            if (sf::FloatRect(childOffsets[i-1], childSize).contains(particle->position)) {
                quadTreeNodes[child_idx].m_index.emplace_back(particle);
                quadTreeNodes[child_idx].totalMass += particle->mass;
                quadTreeNodes[child_idx].com.x += particle->position.x * particle->mass;
                quadTreeNodes[child_idx].com.y += particle->position.y * particle->mass;
                break;
            }
        }
    }

    parentNode.m_index.clear();
}

void QuadTree::deleteTree()
{
    int array[(treeMaxDepth * 4) + 1];

    int top = 0;
    array[top++] = 0;

    while (top > 0) {
        const int currIdx = array[--top];
        QuadTree::Node& currentNode = quadTreeNodes[currIdx];

        if (quadTreeNodes[currIdx].isLeaf) {
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
        QuadTree::Node* currentNode = &quadTreeNodes[currIdx];

        if (currentNode->isLeaf) {
            totalLeafNodes++;

            if (!currentNode->m_index.empty()) {
                vec.push_back(currentNode);


                globalCOM.x += currentNode->com.x;
                globalCOM.y += currentNode->com.y;
                _totalMass += currentNode->totalMass;
            }

            continue;

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
