
#include "Component.h"

#include <algorithm>

static UID __uid = 0;

// ------------------------------------------------------------------------------------------------
QVector3D rotationMod(const QVector3D& rot)
{
  static constexpr float max = 360.0f;

  QVector3D res;
  for (int ii = 0; ii < 3; ++ii)
    res[ii] = fmodf(rot[ii], max);
  return res;
}

// ------------------------------------------------------------------------------------------------
Component::Component() :
    m_uid(++__uid),
    m_name(QString("New Item")),
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
void Component::setName(const QString& name)
{
  m_name = name;
}

// ------------------------------------------------------------------------------------------------
QString Component::name() const
{
  return m_name;
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

  return m_parent->localToWorld() * m_localToParent;
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
const Component* Component::parent() const
{
  return m_parent;
}

// ------------------------------------------------------------------------------------------------
Component* Component::parent()
{
  return m_parent;
}

// ------------------------------------------------------------------------------------------------
void Component::detachFromParent()
{
  if (m_parent == nullptr)
  {
    return;
  }

  auto& children = m_parent->m_children;

  int index = 0;
  for (; index < children.size(); ++index)
  {
    if (children[index]->id() == m_uid)
    {
      break;
    }
  }

  if (index == children.size())
  {
    return;
  }

  children.remove(index);

  m_parent = nullptr;
}

// ------------------------------------------------------------------------------------------------
void Component::setLogger(LogMethod method)
{
  logMethod = method;
}

// ------------------------------------------------------------------------------------------------
void Component::copy(const Pointer& pointer)
{
  m_name = pointer->m_name;
  setLocalToParent(pointer->localPosition(), pointer->localRotation());
  m_matrixMethod = pointer->m_matrixMethod;
  
  m_children.clear();
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
void Component::setAbsolutePosition(const QVector3D& pos)
{
  setLocalToParent(pos - getAbsolutePosition() + m_localPosition, m_localRotation);
}

// ------------------------------------------------------------------------------------------------
QVector3D Component::getAbsolutePosition() const
{
  return localToWorld() * QVector3D(0, 0, 0);
}

// ------------------------------------------------------------------------------------------------
void Component::setAbsoluteRotation(const QVector3D& rot)
{
  setLocalToParent(m_localPosition, rotationMod(rot - getAbsoluteRotation() + m_localRotation));
}

// ------------------------------------------------------------------------------------------------
QVector3D Component::getAbsoluteRotation() const
{
  if (m_parent != nullptr)
  {
    return rotationMod(m_parent->getAbsoluteRotation() + m_localRotation);
  }

  return m_localRotation;
}

// ------------------------------------------------------------------------------------------------
UID Component::id() const
{
  return m_uid;
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
void Component::listenChild(Notification)
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

// ------------------------------------------------------------------------------------------------
void Component::updateTransform()
{
  m_localToParent = m_matrixMethod(m_localPosition, m_localRotation);
}
