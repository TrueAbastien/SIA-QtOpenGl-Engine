
#include "Component.h"

// ------------------------------------------------------------------------------------------------
Component::Component() :
    m_localPosition(QVector3D()),
    m_localRotation(QVector3D()),
    m_localToParent(QMatrix4x4()),
    m_children(0),
    m_parent(nullptr)
{
  const auto method = [](QVector3D pos, QVector3D rot) -> QMatrix4x4
  {
    QMatrix4x4 m = {};
    m.rotate(rot.x(), 1.0, 0.0, 0.0);
    m.rotate(rot.y(), 0.0, 1.0, 0.0);
    m.rotate(rot.z(), 0.0, 0.0, 1.0);
    m.translate(pos);
    return m;
  };
  setMatrixConstruct(method);
}

// ------------------------------------------------------------------------------------------------
void Component::setLocalToParent(const QVector3D& pos, const QVector3D& rot)
{
    m_localPosition = pos;
    m_localRotation = rot;
    m_localToParent = m_matrixMethod(pos, rot);
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
void Component::setLocalPosition(const QVector3D& pos)
{
  setLocalToParent(pos, m_localRotation);
}

// ------------------------------------------------------------------------------------------------
QVector3D Component::localPosition() const
{
  return m_localPosition;
}

// ------------------------------------------------------------------------------------------------
void Component::setLocalRotation(const QVector3D& rot)
{
  setLocalToParent(m_localPosition, rot);
}

// ------------------------------------------------------------------------------------------------
QVector3D Component::localRotation() const
{
  return m_localRotation;
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
void Component::setLogger(LogMethod method)
{
  logMethod = method;
}

// ------------------------------------------------------------------------------------------------
void Component::merge(const Pointer& pointer)
{
  setLocalToParent(pointer->localPosition(), pointer->localRotation());
  
  for (const auto& child : pointer->children())
  {
    addChildren(child);
  }
}

// ------------------------------------------------------------------------------------------------
void Component::setMatrixConstruct(const MatrixConstruct& method)
{
  m_matrixMethod = method;
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

// ------------------------------------------------------------------------------------------------
void Component::log(LogType type, const std::string& message)
{
  if (logMethod)
  {
    logMethod(type, message);
  }
}
