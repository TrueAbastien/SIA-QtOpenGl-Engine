#include "MTAnimatorPlug.h"

#include "MTAnimation.h"

// ------------------------------------------------------------------------------------------------
MTAnimatorPlug::MTAnimatorPlug()
  : m_originalRotation{}, m_currentRotation{}
{
  m_animation = QSharedPointer<MTAnimation>::create();

  MTAnimation::Property prop;
  {
    prop.setter = [&](QVector3D value)
    {
      // TODO: remove one here
      m_currentRotation = value;
      m_parent->setLocalRotation(value);
    };
    prop.keyFrames = {};
  }
  m_animation->addProperty(prop);
}

// ------------------------------------------------------------------------------------------------
void MTAnimatorPlug::init()
{
  if (m_parent == nullptr)
    return;

  auto& kfs = m_animation->property(0).keyFrames;
  QVector3D rot = kfs.front().value;

  m_originalRotation = rot;
  m_currentRotation = rot;
}

// ------------------------------------------------------------------------------------------------
QVector3D MTAnimatorPlug::originalRotation() const
{
  return m_originalRotation;
}

// ------------------------------------------------------------------------------------------------
QVector3D MTAnimatorPlug::currentRotation() const
{
  return m_currentRotation;
}
