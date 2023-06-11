#ifndef QUADTREE
#define QUADTREE
#include <array>
#include <vector>
#include <algorithm>
#include <mutex>
#include "Particle.hpp"

static const int NODE_CAPACITY = 3; //This is the capacity at which the quadtree splits; 1 means it divides until max depth
static const int NODE_MAX_DEPTH = 7;

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
  void update(float dt, Particle* particle);
  void deleteTree();
};

template <typename T>
inline float dot(const sf::Vector2<T>& vec1, const sf::Vector2<T>& vec2);

inline float inv_Sqrt(float number);

// inline bool operator==(const Particle& lhs, const Particle& rhs);
// inline bool operator!=(const X& lhs, const X& rhs){return !operator==(lhs,rhs);}

#endif
