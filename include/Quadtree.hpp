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
    bool isLeaf;

    sf::Vector2f com;
    float totalMass;

    std::vector<Particle*> m_index;
  };

  struct ParticleElementNode {
    int next_particle;
    int particle_index;
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

  void initTree();
  void display(sf::RenderWindow* gameWindow, int totalLeafNodes);
  void insert(Particle* particle);
  void split(const int parentIdx, const sf::Vector2f& childSize, const sf::Vector2f* childOffsets);
  void deleteTree();
  sf::Vector2f getLeafNodes(std::vector<QuadTree::Node*>& vec, int& totalLeafNodes);
  bool empty(const QuadTree::Node*);
  const std::vector<Particle*>& getParticleVec(const QuadTree::Node*);
  const sf::Vector2f& getCOM(const QuadTree::Node*);
  int getTotalMass(const QuadTree::Node*);
  int getMaxDepth();
  int getNodeCap();
  void setMaxDepth(int depth);
};

#endif
