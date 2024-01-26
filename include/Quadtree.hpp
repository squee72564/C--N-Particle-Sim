#ifndef QUADTREE
#define QUADTREE
#include <array>
#include <vector>
#include <stack>
#include <algorithm>
#include <mutex>
#include "Particle.hpp"

/**TODO
 * Change this class into a 1d array structure for the quadtree with size
 * depending on the depth. This will be much more cache friendly than allocating
 * the new nodes on the heap
 *
 * Also the mutex is no longer being used, so we can either use it to synchronize
 * multiple threads inserting into the tree, or get rid of it alltogether
 */

class QuadTree {
private:
  int m_level;
  int treeMaxDepth;
  unsigned int nodeCap;

  float width;
  float height;

  bool isLeaf;

  std::array<QuadTree*, 4> m_subnode;
  
  std::vector<Particle*> m_index;

  sf::RectangleShape m_rect;

  sf::Vector2f com;
  int totalMass;

  std::mutex particleMutex;

public:
  QuadTree();
  //~QuadTree();
  QuadTree(const int m_level, const sf::Vector2f ori, const float h, const float w, const int treeMaxDepth, const int nodeCap);
  QuadTree(const int m_level, const float h, const float w, const int treeMaxDepth, const int nodeCap);
  QuadTree(const QuadTree& qt);
  QuadTree(QuadTree&& qt);
  QuadTree& operator=(const QuadTree& other);  
  QuadTree& operator=(QuadTree&& other);  
  
  void split();
  void display(sf::RenderWindow* gameWindow);
  void insert(Particle* particle);
  void deleteTree();
  void getLeafNodes(std::vector<QuadTree*>& vec);
  bool contains(sf::Vector2f& pos);
  bool empty();
  std::mutex& getParticleMutex();
  std::vector<Particle*>& getParticleVec();
  sf::Vector2f& getCOM();
  int getTotalMass();
  int getMaxDepth();
  int getNodeCap();
};

#endif
