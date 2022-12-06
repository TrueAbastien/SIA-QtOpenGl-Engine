
#include "Component.h"

// ------------------------------------------------------------------------------------------------
Component::Component() :
    m_localToParent(QMatrix4x4()),
    m_children(0),
    m_parent(nullptr)
{
}

// ------------------------------------------------------------------------------------------------
void Component::setLocalToParent(const QVector3D& pos, const QVector3D& rot)
{
    m_localToParent = {};
    m_localToParent.rotate(rot.x(), 1.0, 0.0, 0.0);
    m_localToParent.rotate(rot.y(), 0.0, 1.0, 0.0);
    m_localToParent.rotate(rot.z(), 0.0, 0.0, 1.0);
    m_localToParent.translate(pos);
}

// ------------------------------------------------------------------------------------------------
void Component::setLocalToParent(const QMatrix4x4& localToParent)
{
    m_localToParent = localToParent;
}

// ------------------------------------------------------------------------------------------------
QMatrix4x4 Component::localToParent() const
{
    return m_localToParent;
}

// ------------------------------------------------------------------------------------------------
QMatrix4x4 Component::localToWorld() const
{
  if (m_parent == nullptr)
    return m_localToParent;

  return m_parent->localToParent() * m_localToParent;
}

// ------------------------------------------------------------------------------------------------
void Component::addChildren(const Pointer& child)
{
  m_children.append(child);
  child->setParent(this);
}

// ------------------------------------------------------------------------------------------------
QVector<Component::Pointer> Component::children() const
{
  return m_children;
}

// ------------------------------------------------------------------------------------------------
Component::Pointer Component::child(int index) const
{
  if (index < 0 || index >= m_children.size())
    return nullptr;

  return m_children.at(index);
}

// ------------------------------------------------------------------------------------------------
void Component::setParent(Component* parent)
{
  m_parent = parent;
}

// ------------------------------------------------------------------------------------------------
void Component::notifyParent(Notification notif)
{
  if (m_parent == nullptr) return;

  m_parent->listenChild(notif);
  m_parent->notifyParent(notif);
}

// ------------------------------------------------------------------------------------------------
void Component::listenChild(Notification notif)
{
}
