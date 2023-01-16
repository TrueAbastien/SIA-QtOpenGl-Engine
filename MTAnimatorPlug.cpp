#include "MTAnimatorPlug.h"

#include "MTAnimation.h"

// ------------------------------------------------------------------------------------------------
MTAnimatorPlug::MTAnimatorPlug()
{
  m_animation = QSharedPointer<MTAnimation>::create();

  MTAnimation::Property prop;
  {
    prop.setter = [=](QVector3D value)
    {
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
  m_originalRotation = kfs.front().value;

  for (auto& kf : kfs)
  {
    kf.value -= m_originalRotation;
  }
}
