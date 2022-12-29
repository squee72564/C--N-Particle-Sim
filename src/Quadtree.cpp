#include "QuadTree.hpp"

QuadTree::QuadTree()
  : m_level(0),
    width(1280),
    height(800),
    m_subnode{nullptr, nullptr, nullptr, nullptr},
    isLeaf(true)
{
  m_index.reserve(NODE_CAPACITY);
  m_rect.setPosition(1280-width, 800-height);
  m_rect.setSize(sf::Vector2f(width,height));
  m_rect.setOutlineColor(sf::Color(0,255,0,55));
  m_rect.setOutlineThickness(1);
  m_rect.setFillColor(sf::Color::Transparent);
  origin = sf::Vector2f(0,0);
}

QuadTree::QuadTree(const int m_level, sf::Vector2f ori, float h, float w)
  : m_level(m_level),
    width(w),
    height(h),
    m_subnode{nullptr, nullptr, nullptr, nullptr},
    isLeaf(true)
{
  m_index.reserve(NODE_CAPACITY);
  m_rect.setPosition(ori);
  m_rect.setSize(sf::Vector2f(width,height));
  m_rect.setOutlineColor(sf::Color(0,255,0,55));
  m_rect.setOutlineThickness(1);
  m_rect.setFillColor(sf::Color::Transparent);
  origin = ori;
}

QuadTree::QuadTree(const int m_level, float h, float w)
  : m_level(m_level),
    width(w),
    height(h),
    m_subnode{nullptr, nullptr, nullptr, nullptr},
    isLeaf(true)
{
  m_index.reserve(NODE_CAPACITY);
  m_rect.setPosition(1280-width, 800-height);
  m_rect.setSize(sf::Vector2f(width,height));
  m_rect.setOutlineColor(sf::Color(0,255,0,55));
  m_rect.setOutlineThickness(1);
  m_rect.setFillColor(sf::Color::Transparent);
  origin = sf::Vector2f(0,0);
}

QuadTree::QuadTree(const QuadTree& qt)
  : m_level(qt.m_level),
    width(qt.width),
    height(qt.height),
    m_subnode{qt.m_subnode},
    m_index{qt.m_index},
    isLeaf(qt.isLeaf)
{
  m_index.reserve(NODE_CAPACITY);
  m_rect.setPosition(qt.m_rect.getPosition());
  m_rect.setSize(sf::Vector2f(width,height));
  m_rect.setOutlineColor(sf::Color(0,255,0,55));
  m_rect.setOutlineThickness(1);
  m_rect.setFillColor(sf::Color::Transparent);
  origin = sf::Vector2f(0,0);
}

void QuadTree::split()
{
  if (m_level > NODE_MAX_DEPTH)
  {
    return;
  }

  isLeaf = false;

  m_subnode[0] = new QuadTree(m_level + 1, sf::Vector2f(origin.x, origin.y), height/2, width/2);
  m_subnode[1] = new QuadTree(m_level + 1, sf::Vector2f(origin.x+ width/2, origin.y), height/2, width/2);
  m_subnode[2] = new QuadTree(m_level + 1, sf::Vector2f(origin.x, origin.y + height/2), height/2, width/2);
  m_subnode[3] = new QuadTree(m_level + 1, sf::Vector2f(origin.x + width/2, origin.y + height/2), height/2, width/2);

  for (Particle& particle : m_index)
  {
    for (QuadTree* subNode : m_subnode)
    {
      if (subNode->m_rect.getGlobalBounds().contains(particle.position))
      {
        subNode->insert(particle);
        break;
      }
    }
  }
  m_index.clear();
}

void QuadTree::display(sf::RenderWindow* gameWindow)
{
  if (isLeaf) {
    gameWindow->draw(m_rect);
    return;
  }

  m_subnode[0]->display(gameWindow);
  m_subnode[1]->display(gameWindow);
  m_subnode[2]->display(gameWindow);
  m_subnode[3]->display(gameWindow);
}

void QuadTree::insert(Particle& particle)
{
  //If Quadtree node is a leaf node, insert and split if node is at capacity
  if (isLeaf)
  {
    if (m_index.size() + 1 == NODE_CAPACITY)
    {
      m_index.push_back(particle);
      this->split();
    }
    else
    {
      m_index.push_back(particle);
    }

    return;
  }

  for (QuadTree* subNode : m_subnode) {
    if (subNode->m_rect.getGlobalBounds().contains(particle.position))
    {
      subNode->insert(particle);
      break;
    }
  }
}

void QuadTree::deleteTree()
{
  if (this == nullptr || (m_level == 0 && isLeaf))
  {
    return;
  }

  for (QuadTree* subNode : m_subnode) {
    if (subNode != nullptr && subNode->isLeaf)
    {
      delete subNode;
      subNode = nullptr;
    }
    else if (subNode != nullptr)
    {
      subNode->deleteTree();
    }
  }

  if(m_level == 0)
  {
    m_index.clear();
    isLeaf = true;
  }
}

// QuadTree::QuadTree(QuadTree&& qt)
// {
  
// }