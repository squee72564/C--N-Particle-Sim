#include "QuadTree.hpp"

QuadTree::QuadTree()
  : m_level(0),
    width(1280),
    height(800),
    isLeaf(true),
    m_subnode{nullptr, nullptr, nullptr, nullptr}
{
  m_index.reserve(NODE_CAPACITY + 1);
  m_rect.setPosition(1280-width, 800-height);
  m_rect.setSize(sf::Vector2f(width,height));
  m_rect.setOutlineColor(sf::Color(0,255,0,45));
  m_rect.setOutlineThickness(1);
  m_rect.setFillColor(sf::Color::Transparent);
}

// This constructor is used for all subnodes and takes in a position.
// The origin of each region is the top left corner, and should be the position passed in.
QuadTree::QuadTree(const int m_level, sf::Vector2f position, float w, float h)
  : m_level(m_level),
    width(w),
    height(h),
    isLeaf(true),
    m_subnode{nullptr, nullptr, nullptr, nullptr}
{
  m_index.reserve(NODE_CAPACITY + 1);
  m_rect.setPosition(position);
  m_rect.setSize(sf::Vector2f(width,height));
  m_rect.setOutlineColor(sf::Color(0,255,0,45));
  m_rect.setOutlineThickness(1);
  m_rect.setFillColor(sf::Color::Transparent);
}

//This constructor is used for the root node and does not take in a starting position
QuadTree::QuadTree(const int m_level, float w, float h)
  : m_level(m_level),
    width(w),
    height(h),
    isLeaf(true),
    m_subnode{nullptr, nullptr, nullptr, nullptr}
{
  m_index.reserve(NODE_CAPACITY + 1);
  m_rect.setPosition(0,0);
  m_rect.setSize(sf::Vector2f(width,height));
  m_rect.setOutlineColor(sf::Color(0,255,0,45));
  m_rect.setOutlineThickness(1);
  m_rect.setFillColor(sf::Color::Transparent);
}

QuadTree::QuadTree(const QuadTree& qt)
  : m_level(qt.m_level),
    width(qt.width),
    height(qt.height),
    isLeaf(qt.isLeaf),
    m_subnode{qt.m_subnode},
    m_index{qt.m_index}
{
  m_rect.setPosition(qt.m_rect.getPosition());
  m_rect.setSize(sf::Vector2f(width,height));
  m_rect.setOutlineColor(sf::Color(0,255,0,45));
  m_rect.setOutlineThickness(1);
  m_rect.setFillColor(sf::Color::Transparent);
}

// QuadTree::QuadTree(QuadTree&& qt)
// {
  
// }

void QuadTree::split()
{
  isLeaf = false;

  // Use current origin position of bounding rectangle to calculate origin position for NW,NE,SW,SE subregions
  m_subnode[0] = new QuadTree(m_level + 1, sf::Vector2f(m_rect.getPosition().x, m_rect.getPosition().y), width/2, height/2);
  m_subnode[1] = new QuadTree(m_level + 1, sf::Vector2f(m_rect.getPosition().x + width/2, m_rect.getPosition().y), width/2, height/2);
  m_subnode[2] = new QuadTree(m_level + 1, sf::Vector2f(m_rect.getPosition().x, m_rect.getPosition().y + height/2), width/2, height/2);
  m_subnode[3] = new QuadTree(m_level + 1, sf::Vector2f(m_rect.getPosition().x + width/2, m_rect.getPosition().y + height/2), width/2, height/2);

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

  // If not leaf check subnodes which particle is contained in and insert
  for (QuadTree* subNode : m_subnode) {
    if (subNode != nullptr && subNode->m_rect.getGlobalBounds().contains(particle.position))
    {
      subNode->insert(particle);
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

