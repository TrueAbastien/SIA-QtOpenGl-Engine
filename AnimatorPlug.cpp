#include "AnimatorPlug.h"

// ------------------------------------------------------------------------------------------------
AnimatorPlug::AnimatorPlug()
  : m_animation(QSharedPointer<Animation>::create())
{
}

// ------------------------------------------------------------------------------------------------
void AnimatorPlug::init()
{
  if (m_parent == nullptr)
    return;

  // Position
  QVector3D currPos = m_parent->localPosition();
  for (int ii = 0; ii < 3; ++ii)
  {
    Animation::Property prop;
    {
      prop.setter = [=](float value)
      {
        QVector3D pos = m_parent->localPosition();
        pos[ii] = value;
        m_parent->setLocalPosition(pos);
      };

      Animation::KeyFrame kf;
      {
        kf.time = 0.0f;
        kf.value = currPos[ii];
      }
      prop.keyFrames = {kf};
    }
    m_animation->addProperty(prop);
  }

  // Rotation
  QVector3D currRot = m_parent->localRotation();
  for (int jj = 0; jj < 3; ++jj)
  {
    Animation::Property prop;
    {
      prop.setter = [=](float value)
      {
        QVector3D rot = m_parent->localRotation();
        rot[jj] = value;
        m_parent->setLocalRotation(rot);
      };

      Animation::KeyFrame kf;
      {
        kf.time = 0.0f;
        kf.value = currRot[jj];
      }
      prop.keyFrames = {kf};
    }
    m_animation->addProperty(prop);
  }
}

// ------------------------------------------------------------------------------------------------
void AnimatorPlug::update(UpdateInfo infos)
{
  m_animation->updateProperties(infos.animationTime);
}

// ------------------------------------------------------------------------------------------------
void AnimatorPlug::addKeyFrame(const PropertyType& type, float time, float value)
{
  Animation::KeyFrame kf;
  {
    kf.time = time;
    kf.value = value;
  }
  m_animation->addKeyFrame((int) type, kf);
}
