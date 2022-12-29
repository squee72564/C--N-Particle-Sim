#include "QuadTree.hpp"

QuadTree::QuadTree()
  : m_level(0),
    width(1280),
    height(800),
    m_subnode{nullptr, nullptr, nullptr, nullptr},
    isLeaf(true)
{
  m_index.reserve(NODE_CAPACITY + 1);
  m_rect.setPosition(1280-width, 800-height);
  m_rect.setSize(sf::Vector2f(width,height));
  m_rect.setOutlineColor(sf::Color(0,255,0,45));
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
  m_index.reserve(NODE_CAPACITY + 1);
  m_rect.setPosition(ori);
  m_rect.setSize(sf::Vector2f(width,height));
  m_rect.setOutlineColor(sf::Color(0,255,0,45));
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
  m_index.reserve(NODE_CAPACITY + 1);
  m_rect.setPosition(1280-width, 800-height);
  m_rect.setSize(sf::Vector2f(width,height));
  m_rect.setOutlineColor(sf::Color(0,255,0,45));
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
  m_index.reserve(NODE_CAPACITY + 1);
  m_rect.setPosition(qt.m_rect.getPosition());
  m_rect.setSize(sf::Vector2f(width,height));
  m_rect.setOutlineColor(sf::Color(0,255,0,45));
  m_rect.setOutlineThickness(1);
  m_rect.setFillColor(sf::Color::Transparent);
  origin = sf::Vector2f(0,0);
}

// QuadTree::QuadTree(QuadTree&& qt)
// {
  
// }

void QuadTree::split()
{
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
  //If Quadtree node is a leaf node, insert and split if node greater than capacity
  if (isLeaf)
  {
    m_index.push_back(particle);

    if (m_index.size() > NODE_CAPACITY && m_level <= NODE_MAX_DEPTH)
    {
      this->split();
    }

    return;
  }

  // If not leaf check which subnode particle is contained in an insert into that subnode
  for (QuadTree* subNode : m_subnode) {
    if (subNode != nullptr && subNode->m_rect.getGlobalBounds().contains(particle.position))
    {
      subNode->insert(particle);
      break;
    }
  }
}

void QuadTree::deleteTree()
{
  // If we are at the base node and it is the leaf clear Particle vector and return;
  if (m_level == 0 && isLeaf)
  {
    m_index.clear();
    return;
  }

  // Loop through subNodes for current node of QuadTree
  for (QuadTree* subNode : m_subnode) {

    if (subNode == nullptr) //If one is nullptr all are nullptr so break
    {
      break;
    }
    else if (subNode->isLeaf) //If subnode is a leaf delete
    {
      delete subNode;
      subNode = nullptr;
    }
    else //If not a leaf recursively call down tree, and delete after function returns
    {
      subNode->deleteTree();
      delete subNode;
      subNode = nullptr;
    }
  }

  // Root node is not deleted; just clear Particle vector
  if ( m_level == 0 )
  {
    //m_index.clear();
    isLeaf = true;
  }
}

