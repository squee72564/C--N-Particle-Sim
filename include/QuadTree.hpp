#ifndef QUADTREE
#define QUADTREE

#include <vector>
#include <algorithm>
#include <functional>
#include <utility>

#include "Particle.hpp"

class QuadTree {

public:
  struct Node {
    int firstElement; // Index of first element if leaf and not empty, else -1
    int count;        // Stores number of elements in leaf of -1 if not leaf
    sf::Vector2f com;
    float totalMass;
  };

  struct ParticleElementNode {
    int next_particle;
    int particle_index;

    ParticleElementNode(int next, int idx) : next_particle(next), particle_index(idx) {};
  };

private:
  int w;
  int h;
  int treeMaxDepth;
  unsigned int nodeCap;
  std::vector<QuadTree::Node> quadTreeNodes;
  std::vector<QuadTree::ParticleElementNode> particleNodes;

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
  sf::Vector2f getLeafNodes(std::vector<QuadTree::Node*>& vec, int& totalLeafNodes);
  bool empty(const QuadTree::Node*);
  const std::vector<QuadTree::ParticleElementNode>& getParticleElementNodeVec();
  const sf::Vector2f& getCOM(const QuadTree::Node*);
  int getTotalMass(const QuadTree::Node*);
  int getMaxDepth();
  int getNodeCap();
  void setMaxDepth(int depth);
};

#endif
