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
    int m_level;

    sf::RectangleShape m_rect;

    bool isLeaf;

    sf::Vector2f com;
    float totalMass;

    std::vector<Particle*> m_index;
  };

private:
  int w;
  int h;
  int treeMaxDepth;
  unsigned int nodeCap;
  std::vector<QuadTree::Node> nodes;

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
  void insert(Particle* particle, int index);
  void split(const int parentIdx);
  void deleteTree();
  sf::Vector2f getLeafNodes(std::vector<QuadTree::Node*>& vec, int& totalLeafNodes);
  bool contains(const QuadTree::Node*, const sf::Vector2f& pos);
  bool empty(const QuadTree::Node*);
  const std::vector<Particle*>& getParticleVec(const QuadTree::Node*);
  const sf::Vector2f& getCOM(const QuadTree::Node*);
  int getTotalMass(const QuadTree::Node*);
  int getMaxDepth();
  int getNodeCap();
  void setMaxDepth(int depth);
};

#endif
