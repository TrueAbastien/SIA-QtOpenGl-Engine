#include "AnimatorPlug.h"

// ------------------------------------------------------------------------------------------------
AnimatorPlug::AnimatorPlug()
{
  m_animation = QSharedPointer<Animation<QVector3D>>::create();

  // Position
  {
    AnimationType::Property prop;
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
    AnimationType::Property prop;
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
    AnimationType::KeyFrame kf;
    {
      kf.time = 0.0f;
      kf.value = currPos;
    }
    m_animation->addKeyFrame(0, kf);
  }

  // Rotation
  {
    AnimationType::KeyFrame kf;
    {
      kf.time = 0.0f;
      kf.value = currRot;
    }
    m_animation->addKeyFrame(1, kf);
  }
}
