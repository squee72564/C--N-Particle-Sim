#ifndef QUADTREE
#define QUADTREE
#include <SFML/Graphics.hpp> // Include the SFML graphics library
#include <array>
#include <vector>

#define NODE_CAPACITY 10
#define NODE_MAX_DEPTH 6

class QuadTree {
public:
  int m_level;
  float height;
  float width;
  sf::RectangleShape m_rect;
  sf::Vector2f origin;
  std::array<QuadTree*, 4> m_subnode;
  std::vector<int> m_index;
  bool isLeaf;

  //void split();
  //bool contain() const;
  //bool contain_rect() const;
  //void set_color();

  QuadTree(const int m_level, sf::Vector2f ori,  float h, float w);
  void insert();

  //void update();
  //void get() const;
  //void retrieve() const;
  //void draw() const;
  //void reset();
};

extern QuadTree quadtree;

#endif