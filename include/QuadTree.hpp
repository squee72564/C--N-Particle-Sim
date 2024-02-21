#ifndef QUADTREE
#define QUADTREE

#include <vector>
#include <algorithm>
#include <functional>
#include <utility>

#include "Particle.hpp"
#include "Helpers.hpp"

class QuadTree {

public:
  struct TreeNode {
    int firstParticle;      // Index of first element if leaf and not empty, else -1
    int gravElement;		// index of the gravity node for this quadtree cell
    int count;              // Stores number of elements in leaf or -1 if it is a branch

    TreeNode() : firstParticle(-1), gravElement(-1), count(0) {};
  };

  struct GravityElementNode {
    double com_x;
    double com_y;
    double totalMass;

    GravityElementNode() = default;
    GravityElementNode(double com, double mass) : com_x(com), com_y(com), totalMass(mass) {};
    ~GravityElementNode() = default;
  };

  struct ParticleElementNode {
    int next_particle;
    int particle_index;

    ParticleElementNode() = default;
    ParticleElementNode(int next, int idx) : next_particle(next), particle_index(idx) {};
    ~ParticleElementNode() = default;
  };

private:
  int w;
  int h;
  int treeMaxDepth;
  unsigned int nodeCap;
  std::vector<QuadTree::TreeNode> quadTreeNodes;
  std::vector<QuadTree::ParticleElementNode> particleNodes;
  FreeList<QuadTree::GravityElementNode> gravityNodes;

public:
  QuadTree();
  QuadTree(const int w, const int h, const int maxDepth, const int capacity);
  QuadTree(const QuadTree& other);
  QuadTree(QuadTree&& other) noexcept;
  QuadTree& operator=(const QuadTree& other);
  QuadTree& operator=(QuadTree&& other) noexcept;
  ~QuadTree();

  void display(sf::RenderWindow* gameWindow, int totalLeafNodes);
  void insert(std::vector<Particle>& particles);
  void split(const int parentIdx, const sf::Vector2f& childSize, const sf::Vector2f* childOffsets, std::vector<Particle>& particles);
  void deleteTree();
  sf::Vector2f getLeafNodes(std::vector<QuadTree::TreeNode*>& vec, int& totalLeafNodes, float& _totalMass);
  bool empty(const QuadTree::TreeNode*);
  const std::vector<QuadTree::ParticleElementNode>& getParticleElementNodeVec();
  const sf::Vector2f getCOM(const QuadTree::TreeNode*);
  int getTotalMass(const QuadTree::TreeNode*);
  int getMaxDepth();
  int getNodeCap();
  void setMaxDepth(int depth);
};

#endif
