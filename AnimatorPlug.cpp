#include "AnimatorPlug.h"

// ------------------------------------------------------------------------------------------------
AnimatorPlug::AnimatorPlug()
{
  m_animation = QSharedPointer<Animation>::create();

  // Position
  {
    Animation::Property prop;
    {
      prop.setter = [=](QVector3D value)
      {
        m_parent->setLocalPosition(value);
      };
      prop.keyFrames = {};
    }
    m_animation->addProperty(prop);
  }

  // Rotation
  {
    Animation::Property prop;
    {
      prop.setter = [=](QVector3D value)
      {
        m_parent->setLocalRotation(value);
      };
      prop.keyFrames = {};
    }
    m_animation->addProperty(prop);
  }
}

// ------------------------------------------------------------------------------------------------
void AnimatorPlug::init()
{
  if (m_parent == nullptr)
    return;

  QVector3D currPos = m_parent->localPosition();
  QVector3D currRot = m_parent->localRotation();

  // Position
  {
    Animation::KeyFrame kf;
    {
      kf.time = 0.0f;
      kf.value = currPos;
    }
    m_animation->addKeyFrame(0, kf);
  }

  // Rotation
  {
    Animation::KeyFrame kf;
    {
      kf.time = 0.0f;
      kf.value = currRot;
    }
    m_animation->addKeyFrame(1, kf);
  }
}
