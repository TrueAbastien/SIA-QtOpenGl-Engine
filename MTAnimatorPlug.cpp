#include "MTAnimatorPlug.h"

#include "MTAnimation.h"

// ------------------------------------------------------------------------------------------------
MTAnimatorPlug::MTAnimatorPlug()
  : m_originalRotation{}
{
  m_animation = QSharedPointer<MTAnimation>::create();

  // Rotation
  {
    MTAnimation::Property prop;
    {
      prop.setter = [&](QVector3D value)
      {
        m_parent->setLocalRotation(value);
      };
      prop.keyFrames = {};
    }
    m_animation->addProperty(prop);
  }

  // Position
  {
    MTAnimation::Property prop;
    {
      prop.setter = [&](QVector3D value)
      {
        m_parent->setLocalPosition(value);
      };
      prop.keyFrames = {};
    }
    m_animation->addProperty(prop);
  }
}

// ------------------------------------------------------------------------------------------------
void MTAnimatorPlug::init()
{
  if (m_parent == nullptr)
    return;

  auto& kfs = m_animation->property(0).keyFrames;
  QVector3D rot = kfs.front().value;

  m_originalRotation = rot;
}

// ------------------------------------------------------------------------------------------------
QVector3D MTAnimatorPlug::originalRotation() const
{
  return m_originalRotation;
}
