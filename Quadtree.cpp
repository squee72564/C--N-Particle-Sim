#include "QuadTree.hpp"

QuadTree::QuadTree(const int m_level, sf::Vector2f ori, float h, float w)
  : m_level(m_level),
    width(w),
    height(h),
    origin(ori),
    m_subnode{nullptr, nullptr, nullptr, nullptr},
    m_index{NULL}
{
  m_index.reserve(NODE_CAPACITY);
  m_rect.setPosition(sf::Vector2f(ori.x-(w/2),ori.y-(h/2) ));
  m_rect.setSize(sf::Vector2f(width,height));
  m_rect.setOutlineColor(sf::Color::Blue);
  m_rect.setOutlineThickness(1);
  m_rect.setFillColor(sf::Color::Transparent);
}