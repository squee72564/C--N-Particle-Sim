#ifndef QUADTREE
#define QUADTREE
#include <SFML/Graphics.hpp> // Include the SFML graphics library
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include "Particle.hpp"

static const int NODE_CAPACITY = 1; //This is the capacity at which the quadtree splits; 1 means it divides until max depthj
static const int NODE_MAX_DEPTH = 6;

class QuadTree {
private:
  int m_level;

  float width;
  float height;

  bool isLeaf;

  std::array<QuadTree*, 4> m_subnode;
  std::vector<Particle> m_index;

  sf::RectangleShape m_rect;

  //sf::Vector2f com;

public:

  QuadTree();
  //~QuadTree();
  QuadTree(const int m_level, sf::Vector2f ori, float h, float w);
  QuadTree(const int m_level, float h, float w);
  QuadTree(const QuadTree& qt);
  //QuadTree(QuadTree&& qt);
  
  void split();
  void display(sf::RenderWindow* gameWindow);
  void insert(Particle& particle);
  void deleteTree();
};

inline bool operator==(const Particle& lhs, const Particle& rhs);
//inline bool operator!=(const X& lhs, const X& rhs){return !operator==(lhs,rhs);}

#endif