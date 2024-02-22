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
	int index;
    int depth;
    sf::Vector2f p;
    sf::Vector2f s;
};

QuadTree::QuadTree()
  : w_(1920),
    h_(1080),
    tree_max_depth_(7),
    node_cap_(4)
{
    std::cout << "Default Constructor for QuadTree called.\n";
}

QuadTree::QuadTree(const int w, const int h, const int max_depth, const int capacity)
  : w_(w),
    h_(h),
    tree_max_depth_(max_depth),
    node_cap_(capacity)
{
    std::cout << "Non-default Constructor for QuadTree called.\n";
    
    // Set up vector that holds particle element nodes for the tree leaves
    particle_nodes_.reserve(std::pow(4,max_depth) * capacity);
    gravity_nodes_.reserve(std::pow(4,max_depth));

    // Initially set up vector for quadtreenodes, this will depend on max depth
    int reserved = calculateTotalNodes(max_depth);
    tree_nodes_ = std::vector<QuadTree::TreeNode>(reserved, QuadTree::TreeNode());
    
    std::cout << "Tree initialized with " << tree_nodes_.size() << " nodes.\n";
}

QuadTree::QuadTree(const QuadTree& other)
{
    std::cout << "Copy Constructor called\n";
    w_ = other.w_;
    h_ = other.h_;
    tree_max_depth_ = other.tree_max_depth_;
    node_cap_ = other.node_cap_;
    tree_nodes_ = other.tree_nodes_;
}

QuadTree::QuadTree(QuadTree&& other) noexcept
{
    std::cout << "Move Constructor called\n";
    w_ = std::exchange(other.w_, 1920);
    h_ = std::exchange(other.h_, 1080);
    tree_max_depth_ = std::exchange(other.tree_max_depth_, 0);
    node_cap_ = std::exchange(other.node_cap_, 0);
    tree_nodes_ = std::move(other.tree_nodes_);
}

QuadTree& QuadTree::operator=(const QuadTree& other)
{
    std::cout << "Copy assignment called\n";
    if (this != &other) {
        w_ = other.w_;
        h_ = other.h_;
        tree_max_depth_ = other.tree_max_depth_;
        node_cap_ = other.node_cap_;
        tree_nodes_ = other.tree_nodes_;
    }

    return *this;
}
 
QuadTree& QuadTree::operator=(QuadTree&& other) noexcept
{
    std::cout << "Move assignment called\n";
    if (this != &other) {
        w_ = std::exchange(other.w_, 1920);
        h_ = std::exchange(other.h_, 1080);
        tree_max_depth_ = std::exchange(other.tree_max_depth_, 0);
        node_cap_ = std::exchange(other.node_cap_, 0);
        tree_nodes_ = std::move(other.tree_nodes_);
    }

    return *this;
}

QuadTree::~QuadTree()
{
    std::cout << "Destructor called\n";
    tree_nodes_.clear();
}

void QuadTree::display(sf::RenderWindow* game_window, int total_leaf_nodes)
{
    const int n = (total_leaf_nodes * 8); 
    sf::VertexArray lines(sf::Lines, n); // this is how many lines we need for all grids
    long vi = 0;

    const sf::Color colors[4] = {
        sf::Color(0,0,204,35),
        sf::Color(102,0,204,35),
        sf::Color(0,102,204,35),
        sf::Color(255,0,127,35),
    };

    NodeData array[40]; // <idx, nodeInfo>

    int top = 0;
    
    NodeData node;
	node.index = 0;
    node.depth = 0;
    node.p = sf::Vector2f(0.0f, 0.0f);
    node.s = sf::Vector2f(w_, h_);

    array[top++] = node;
    
    while (top > 0) {
        const int curr_index = array[--top].index;
        const int curr_depth = array[top].depth;
        const sf::Vector2f curr_pos = array[top].p;
        const sf::Vector2f curr_size = array[top].s;

        const QuadTree::TreeNode& current_node = tree_nodes_[curr_index];

        if (current_node.count != -1) {

            const sf::Vector2f positions[4] = {
                curr_pos,
                sf::Vector2f(curr_pos.x + curr_size.x, curr_pos.y),
                sf::Vector2f(curr_pos.x + curr_size.x, curr_pos.y + curr_size.y),
                sf::Vector2f(curr_pos.x, curr_pos.y + curr_size.y),
            };
            
            for (int i = 0; i < 4; ++i) {
                lines[vi].position= positions[i];
                lines[vi++].color = colors[i];

                lines[vi].position = positions[(i+1)%4];
                lines[vi++].color = colors[(i+1)%4];
            }


        } else {

            const sf::Vector2f child_size(curr_size.x * 0.5f, curr_size.y * 0.5f);
            const sf::Vector2f offsets[4] = {
                curr_pos,
                sf::Vector2f(curr_pos.x + child_size.x, curr_pos.y), 
                sf::Vector2f(curr_pos.x, curr_pos.y + child_size.y), 
                sf::Vector2f(curr_pos.x + child_size.x, curr_pos.y + child_size.y), 
            };

            for (int i = 1; i <= 4; ++i) {
                const int child_idx = 4 * curr_index + i;

                node = {child_idx, curr_depth+1, offsets[i-1], child_size};

                array[top++] = node;
            }
        }
    }
    game_window->draw(lines);
}

void QuadTree::insert(std::vector<Particle>& particles)
{

    NodeData array[40]; // <idx, nodeInfo>
    
    // Create the first gravity node for the root, and the split function will add and remove
    // nodes as needed
    tree_nodes_[0].grav_element = gravity_nodes_.insert(QuadTree::GravityElementNode());


    for (std::size_t i = 0; i < particles.size(); ++i) {
        

        int top = 0;
        
        NodeData node;
		node.index = 0;
        node.depth = 0;
        node.p = sf::Vector2f(0.0f, 0.0f);
        node.s = sf::Vector2f(w_, h_);

        array[top++] = node;
        
        while (top > 0) {
            const int curr_index = array[--top].index;
            const int curr_depth = array[top].depth;
            const sf::Vector2f curr_pos = array[top].p;
            const sf::Vector2f curr_size = array[top].s;

            QuadTree::TreeNode& currNode = tree_nodes_[curr_index];

            const sf::Vector2f child_size(curr_size.x * 0.5f, curr_size.y * 0.5f);
            const sf::Vector2f offsets[4] = {
                curr_pos,
                sf::Vector2f(curr_pos.x + child_size.x, curr_pos.y), 
                sf::Vector2f(curr_pos.x, curr_pos.y + child_size.y), 
                sf::Vector2f(curr_pos.x + child_size.x, curr_pos.y + child_size.y), 
            };

            if (currNode.count != -1) { // If current node is a leaf
                const size_t num_particles = currNode.count;

                if (curr_depth < tree_max_depth_ && num_particles+1 > node_cap_) {
                    split(curr_index, child_size, offsets, particles);
                } else {
                    particle_nodes_.emplace_back(QuadTree::ParticleElementNode(currNode.first_particle, i));
                    currNode.first_particle = particle_nodes_.size() - 1;
                    currNode.count++;
                    
                    assert(currNode.grav_element >= 0);

                    QuadTree::GravityElementNode& gNode = gravity_nodes_[currNode.grav_element];

                    gNode.total_mass += particles[i].mass;
                    gNode.com_x += particles[i].position.x * particles[i].mass;
                    gNode.com_y += particles[i].position.y * particles[i].mass;
                    continue;
                }
            }

            for (int j = 1; j <= 4; j++) {
                const int child_idx = 4 * curr_index + j;

                if (sf::FloatRect(offsets[j-1], child_size).contains(particles[i].position)) {
                    node = {child_idx, curr_depth+1, offsets[j-1], child_size};

                    array[top++] = node;

                    break;
                }
            }
        }
    }
}

void QuadTree::split(const int parent_index,
					 const sf::Vector2f& child_size,
					 const sf::Vector2f* child_offsets,
					 std::vector<Particle>& particles)
{
    QuadTree::TreeNode& parent_tree_node = tree_nodes_[parent_index];

    gravity_nodes_.erase(parent_tree_node.grav_element);

    int curr_particle_element_index = parent_tree_node.first_particle;

    while (curr_particle_element_index != -1) {
        QuadTree::ParticleElementNode& curr_particle_element = particle_nodes_[curr_particle_element_index];

        int next_particle_element = curr_particle_element.next_element_index;
        
        Particle& curr_particle = particles[curr_particle_element.particle_index];

        for (int i = 1; i <= 4; ++i) {
            const int child_idx = 4 * parent_index + i;
            tree_nodes_[child_idx].grav_element = gravity_nodes_.insert(QuadTree::GravityElementNode());
        }

        for (int i = 1; i <= 4; ++i) {
            const int child_idx = 4 * parent_index + i;
            if (sf::FloatRect(child_offsets[i-1], child_size).contains(curr_particle.position)) {
                
                QuadTree::TreeNode& child_tree_node = tree_nodes_[child_idx];
                curr_particle_element.next_element_index = child_tree_node.first_particle;
                child_tree_node.first_particle = curr_particle_element_index;
                child_tree_node.count++;
                
				QuadTree::GravityElementNode& child_gravity_element = gravity_nodes_[child_tree_node.grav_element];
                child_gravity_element.total_mass += curr_particle.mass;
                child_gravity_element.com_x += curr_particle.position.x * curr_particle.mass;
                child_gravity_element.com_y += curr_particle.position.y * curr_particle.mass;
                break;
            }
        }

        curr_particle_element_index = next_particle_element;
    }
	
    parent_tree_node.count = -1;
}

void QuadTree::deleteTree()
{
    std::fill(tree_nodes_.begin(), tree_nodes_.end(), QuadTree::TreeNode());

    particle_nodes_.clear(); //  Clear all particle element nodes as they will be re-inserted next frame
    gravity_nodes_.clear();
}

sf::Vector2f QuadTree::getLeafNodes(std::vector<QuadTree::TreeNode*>& vec, int& total_leaf_nodes, float& total_mass)
{
    sf::Vector2f global_com(0,0);

    int array[40];

    int top = 0;
    array[top++] = 0;

    // reset max depth tracking variable
    total_leaf_nodes = 0;

    while (top > 0) {
        const int curr_index = array[--top];
        QuadTree::TreeNode* current_node = &tree_nodes_[curr_index];

        if (current_node->count != -1) {
            total_leaf_nodes++;

            if (current_node->count > 0) {
                vec.push_back(current_node);

                QuadTree::GravityElementNode& gNode = gravity_nodes_[current_node->grav_element];
                global_com.x += gNode.com_x;
                global_com.y += gNode.com_y;
                total_mass += gNode.total_mass;
            }

        } else {
            for (int i = 1; i <= 4; ++i) {
                const int child_idx = 4 * curr_index + i;
                array[top++] = child_idx;
            }
        }
    }

    if (total_mass > 0.0f) {
        global_com.x /= total_mass;
        global_com.y /= total_mass;
    } else {
        global_com.x = 0.0f;
        global_com.y = 0.0f;
    }
    return global_com;
}

bool QuadTree::empty(const QuadTree::TreeNode* node)
{
    return (node->count == 0);
}

const std::vector<QuadTree::ParticleElementNode>& QuadTree::getParticleElementNodeVec()
{
    return particle_nodes_;
}

const sf::Vector2f QuadTree::getCOM(const QuadTree::TreeNode* node)
{
    float x = gravity_nodes_[node->grav_element].com_x;
    float y = gravity_nodes_[node->grav_element].com_y;
    return sf::Vector2f(x,y);
}

int QuadTree::getTotalMass(const QuadTree::TreeNode* node)
{
    return gravity_nodes_[node->grav_element].total_mass;
}

int QuadTree::getMaxDepth()
{
    return tree_max_depth_;
}

int QuadTree::getNodeCap()
{
    return tree_max_depth_;
}

void QuadTree::setMaxDepth(int depth)
{
    tree_max_depth_ = depth;
}
