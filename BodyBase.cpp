#include "BodyBase.h"

// ------------------------------------------------------------------------------------------------
BodyBase::Hierarchy::Node::Pointer BodyBase::Hierarchy::root() const
{
  return m_root;
}

// ------------------------------------------------------------------------------------------------
void invertNodeHierarchy(
  const BodyBase::Hierarchy::Node::Pointer& previous,
  const BodyBase::Hierarchy::Node::Pointer& node)
{
  if (node.isNull())
  {
    return;
  }

  invertNodeHierarchy(node, node->parent);

  node->parent = previous;
  node->children.removeAll(previous);

  if (!previous.isNull())
  {
    previous->children.append(node);
  }
}
bool BodyBase::Hierarchy::makeRoot(const Node::Pointer& node)
{
  if (node == nullptr || !node->children.isEmpty())
  {
    return false;
  }

  if (m_root == node)
  {
    return true;
  }

  invertNodeHierarchy(nullptr, node);

  return true;
}

// ------------------------------------------------------------------------------------------------
BodyBase::Hierarchy::Node::Pointer BodyBase::Hierarchy::createNode(
  const Component* joint,
  const Node::Pointer& parent)
{
  if (joint == nullptr)
  {
    return nullptr;
  }

  auto node = Node::Pointer::create();
  node->joint = joint;

  if (parent != nullptr)
  {
    parent->children.append(node);
    node->parent = parent;
  }
  else
  {
    node->parent = nullptr;
  }

  if (m_root.isNull())
  {
    m_root = node;
  }

  return node;
}

// ------------------------------------------------------------------------------------------------
BodyBase::Hierarchy::Pointer BodyBase::hierarchy()
{
  if (m_cachedHierarchy.isNull())
  {
    m_cachedHierarchy = computeHierarchy();
  }

  return m_cachedHierarchy;
}

// ------------------------------------------------------------------------------------------------
void BodyBase::resetHierarchy()
{
  m_cachedHierarchy = nullptr;
}
