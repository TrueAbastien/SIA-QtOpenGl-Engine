#include "MTAnimatorPlug.h"

#include "MTAnimation.h"

// ------------------------------------------------------------------------------------------------
MTAnimatorPlug::MTAnimatorPlug()
{
  m_animation = QSharedPointer<MTAnimation>::create();

  MTAnimation::Property prop;
  {
    prop.setter = [=](QQuaternion value)
    {
      QVector3D rot;
      {
        float x, y, z;
        value.getEulerAngles(&z, &y, &x);
        rot = {x,y,z};
      }
      m_parent->setAbsoluteRotation(rot);
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

  /*MTAnimation::KeyFrame kf;
  {
    kf.time = 0.0f;
    kf.value = QQuaternion();
  }
  m_animation->addKeyFrame(0, kf);*/
}
