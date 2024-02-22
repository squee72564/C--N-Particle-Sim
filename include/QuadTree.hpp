#ifndef QUADTREE
#define QUADTREE

#include <vector>       // std::vector
#include <functional>   // std::fill()
#include <utility>      // std::exchange()
#include <cmath>        // std::pow()

#include "Particle.hpp"
#include "Helpers.hpp"

class QuadTree {

public:
  struct TreeNode {
    int first_particle;     // Index of first element if leaf and not empty, else -1
    int grav_element;		// index of the gravity node for this quadtree cell
    int count;              // Stores number of elements in leaf or -1 if it is a branch

    TreeNode() : first_particle(-1), grav_element(-1), count(0) {};
  };

  struct GravityElementNode {
    double com_x;
    double com_y;
    double total_mass;

    GravityElementNode() : com_x(0.0f), com_y(0.0f), total_mass(0.0f) {}
    //GravityElementNode(double com, double mass) : com_x(com), com_y(com), total_mass(mass) {}
    ~GravityElementNode() = default;
  };

  struct ParticleElementNode {
    int next_element_index;
    int particle_index;

    ParticleElementNode() = default;
    ParticleElementNode(int next, int idx) : next_element_index(next), particle_index(idx) {};
    ~ParticleElementNode() = default;
  };

private:
  int w_;
  int h_;
  int tree_max_depth_;
  unsigned int node_cap_;
  std::vector<QuadTree::TreeNode> tree_nodes_;
  std::vector<QuadTree::ParticleElementNode> particle_nodes_;
  FreeList<QuadTree::GravityElementNode> gravity_nodes_;

public:
  QuadTree();
  QuadTree(const int w, const int h, const int max_depth, const int capacity);
  QuadTree(const QuadTree& other);
  QuadTree(QuadTree&& other) noexcept;
  QuadTree& operator=(const QuadTree& other);
  QuadTree& operator=(QuadTree&& other) noexcept;
  ~QuadTree();

  void display(sf::RenderWindow* game_window, int total_leaf_nodes);
  void insert(std::vector<Particle>& particles);
  void split(const int parent_index,
             const sf::Vector2f& child_size,
             const sf::Vector2f(& child_offsets)[4],
             std::vector<Particle>& particles);
  void deleteTree();
  sf::Vector2f getLeafNodes(std::vector<QuadTree::TreeNode*>& vec,
                            int& total_leaf_nodes,
                            float& total_mass);
  bool empty(const QuadTree::TreeNode* node);
  const std::vector<QuadTree::ParticleElementNode>& getParticleElementNodeVec();
  const sf::Vector2f getCOM(const QuadTree::TreeNode* node);
  int getTotalMass(const QuadTree::TreeNode* node);
  int getMaxDepth();
  int getNodeCap();
  void setMaxDepth(int depth);
};

#endif
