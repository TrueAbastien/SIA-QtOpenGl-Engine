
#pragma once

#include "WiredRenderable.h"

#include <QMap>
#include <QVector>

class BodyBase : public WiredRenderable
{
public:

  class Hierarchy
  {
  public:

    struct Node
    {
      using Pointer = QSharedPointer<Node>;
      using Vector = QVector<Pointer>;

      const Component* joint;

      Node::Pointer parent;
      Node::Vector children;
    };

  public:

    using Pointer = QSharedPointer<Hierarchy>;

  public:

    Node::Pointer root() const;

    bool makeRoot(const Node::Pointer& node);
    Node::Pointer createNode(const Component* joint, const Node::Pointer& parent);

  private:

    Node::Pointer m_root = nullptr;
  };

protected:

  BodyBase() = default;

public:

  Hierarchy::Pointer hierarchy();

  virtual Hierarchy::Pointer computeHierarchy() const = 0;
  void resetHierarchy();

protected:

  Hierarchy::Pointer m_cachedHierarchy = nullptr;
};
