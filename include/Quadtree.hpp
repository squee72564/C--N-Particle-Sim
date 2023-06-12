#ifndef QUADTREE
#define QUADTREE
#include <array>
#include <vector>
#include <algorithm>
#include <mutex>
#include "Particle.hpp"

static int NODE_CAPACITY = 3; //This is the capacity at which the quadtree splits; 1 means it divides until max depth
static int NODE_MAX_DEPTH = 5;

class QuadTree {
private:
  int m_level;

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
  QuadTree(const int m_level, sf::Vector2f ori, float h, float w);
  QuadTree(const int m_level, float h, float w);
  QuadTree(const QuadTree& qt);
  QuadTree(QuadTree&& qt);
  QuadTree& operator=(const QuadTree& other);  
  QuadTree& operator=(QuadTree&& other);  
  
  void split();
  void display(sf::RenderWindow* gameWindow);
  void insert(Particle* particle);
  void updateForces(float dt, Particle* particle);
  void deleteTree();
  void getLeafNodes(std::vector<QuadTree*>& vec);
  bool contains(sf::Vector2f& pos);
  bool empty();
  std::mutex& getParticleMutex();
  std::vector<Particle*>& getParticleVec();
  sf::Vector2f& getCOM();
  int& getTotalMass();
};

template <typename T>
inline float dot(const sf::Vector2<T>& vec1, const sf::Vector2<T>& vec2);

inline float inv_Sqrt(float number);

#endif
