#include <iostream>

#include "QuadTree.hpp"

static inline int calculateTotalNodes(const int depth) {
    int total = 0;
    for (int i = 0; i <= depth; ++i) {
        total += std::pow(4,i);
    }
    return total;
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
    
    // Set up vector that holds particle element nodes for the tree leaves
    particleNodes.reserve(std::pow(4,maxDepth) * capacity);
    gravityNodes.reserve(std::pow(4,maxDepth));

    // Initially set up vector for quadtreenodes, this will depend on max depth
    int reserved = calculateTotalNodes(maxDepth);
    quadTreeNodes = std::vector<QuadTree::Node>(reserved, QuadTree::Node());
    
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

void QuadTree::display(sf::RenderWindow* gameWindow, int totalLeafNodes)
{
    const int n = (totalLeafNodes * 8); 
    sf::VertexArray lines(sf::Lines, n); // this is how many lines we need for all grids
    long vi = 0;

    const sf::Color colors[4] = {
        sf::Color(0,0,204,35),
        sf::Color(102,0,204,35),
        sf::Color(0,102,204,35),
        sf::Color(255,0,127,35),
    };

    std::pair<int, NodeData> array[40]; // <idx, nodeInfo>

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

        if (currentNode.count != -1) {

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

            const sf::Vector2f cSize(currS.x * 0.5f, currS.y * 0.5f);
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

void QuadTree::insert(std::vector<Particle>& particles)
{

    std::pair<int, NodeData> array[40]; // <idx, nodeInfo>
    
    // Create the first gravity node for the root, and the split function will add and remove
    // nodes as needed
    quadTreeNodes[0].gravElement = gravityNodes.insert(QuadTree::GravityElementNode(0.0f, 0.0f));


    for (std::size_t i = 0; i < particles.size(); ++i) {
        

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

            QuadTree::Node& currNode = quadTreeNodes[currIdx];

            const sf::Vector2f cSize(currS.x * 0.5f, currS.y * 0.5f);
            const sf::Vector2f offsets[4] = {
                currP,
                sf::Vector2f(currP.x + cSize.x, currP.y), 
                sf::Vector2f(currP.x, currP.y + cSize.y), 
                sf::Vector2f(currP.x + cSize.x, currP.y + cSize.y), 
            };

            if (currNode.count != -1) { // If current node is a leaf
                const size_t particleNum = currNode.count;

                if (currDepth < treeMaxDepth && particleNum+1 > nodeCap) {
                    split(currIdx, cSize, offsets, particles);
                } else {
                    particleNodes.emplace_back(QuadTree::ParticleElementNode(currNode.firstElement, i));
                    currNode.firstElement = particleNodes.size() - 1;
                    currNode.count++;
                    
                    assert(currNode.gravElement >= 0);

                    QuadTree::GravityElementNode& gNode = gravityNodes[currNode.gravElement];

                    gNode.totalMass += particles[i].mass;
                    gNode.com_x += particles[i].position.x * particles[i].mass;
                    gNode.com_y += particles[i].position.y * particles[i].mass;
                    continue;
                }
            }

            for (int j = 1; j <= 4; j++) {
                const int child_idx = 4 * currIdx + j;

                if (sf::FloatRect(offsets[j-1], cSize).contains(particles[i].position)) {
                    node = {currDepth+1, offsets[j-1], cSize};

                    nodePair.first = child_idx;
                    nodePair.second = node;

                    array[top++] = nodePair;

                    break;
                }
            }
        }
    }
}

void QuadTree::split(const int parentIdx, const sf::Vector2f& childSize, const sf::Vector2f* childOffsets, std::vector<Particle>& particles)
{
    QuadTree::Node& parentNode = quadTreeNodes[parentIdx];

    // Delete the gravity node for the parent from the free list
    gravityNodes.erase(parentNode.gravElement);

    // Traverse the linked list of particles within the leaf and
    // place into children
    int currParticleElementNodeIdx = parentNode.firstElement;

    while (currParticleElementNodeIdx != -1) {
        // Get the current particle element node
        QuadTree::ParticleElementNode& currElementNode = particleNodes[currParticleElementNodeIdx];

        // Save next particle index
        int nextParticleElementNodeIdx = currElementNode.next_particle;
        
        // Get the actual particle for this element node
        Particle& currParticle = particles[currElementNode.particle_index];

        // Insert gravity nodes for all children
        for (int i = 1; i <= 4; ++i) {
            const int child_idx = 4 * parentIdx + i;
            quadTreeNodes[child_idx].gravElement = gravityNodes.insert(QuadTree::GravityElementNode(0.0f, 0.0f));
        }

        for (int i = 1; i <= 4; ++i) {
            const int child_idx = 4 * parentIdx + i;
            if (sf::FloatRect(childOffsets[i-1], childSize).contains(currParticle.position)) {
                
                QuadTree::Node& child = quadTreeNodes[child_idx];
                currElementNode.next_particle = child.firstElement;
                child.firstElement = currParticleElementNodeIdx;
                child.count++;
                
                gravityNodes[child.gravElement].totalMass += currParticle.mass;
                gravityNodes[child.gravElement].com_x += currParticle.position.x * currParticle.mass;
                gravityNodes[child.gravElement].com_y += currParticle.position.y * currParticle.mass;
                break;
            }
        }

        // Continue to next particle
        currParticleElementNodeIdx = nextParticleElementNodeIdx;
    }
        
    // Reset Parent node and mark as branch
    parentNode.count = -1;
}

void QuadTree::deleteTree()
{
    const QuadTree::Node t = QuadTree::Node();
    std::fill(quadTreeNodes.begin(), quadTreeNodes.end(), t);

    particleNodes.clear(); //  Clear all particle element nodes as they will be re-inserted next frame
    gravityNodes.clear();
}

sf::Vector2f QuadTree::getLeafNodes(std::vector<QuadTree::Node*>& vec, int& totalLeafNodes, float& _totalMass)
{
    sf::Vector2f globalCOM(0,0);

    int array[40];

    int top = 0;
    array[top++] = 0;

    // reset max depth tracking variable
    totalLeafNodes = 0;

    while (top > 0) {
        const int currIdx = array[--top];
        QuadTree::Node* currentNode = &quadTreeNodes[currIdx];

        if (currentNode->count != -1) {
            totalLeafNodes++;

            if (currentNode->count > 0) {
                vec.push_back(currentNode);

                QuadTree::GravityElementNode& gNode = gravityNodes[currentNode->gravElement];
                globalCOM.x += gNode.com_x;
                globalCOM.y += gNode.com_y;
                _totalMass += gNode.totalMass;
            }

        } else {
            for (int i = 1; i <= 4; ++i) {
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
    return (node->count == 0);
}

const std::vector<QuadTree::ParticleElementNode>& QuadTree::getParticleElementNodeVec()
{
    return particleNodes;
}

const sf::Vector2f QuadTree::getCOM(const QuadTree::Node* node)
{
    float x = gravityNodes[node->gravElement].com_x;
    float y = gravityNodes[node->gravElement].com_y;
    return sf::Vector2f(x,y);
}

int QuadTree::getTotalMass(const QuadTree::Node* node)
{
    return gravityNodes[node->gravElement].totalMass;
}

int QuadTree::getMaxDepth()
{
    return treeMaxDepth;
}

int QuadTree::getNodeCap()
{
    return treeMaxDepth;
}

void QuadTree::setMaxDepth(int depth)
{
    treeMaxDepth = depth;
}
