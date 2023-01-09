#include "MTAnimatorPlug.h"

#include "MTAnimation.h"

// ------------------------------------------------------------------------------------------------
MTAnimatorPlug::MTAnimatorPlug()
  : m_parentAcceleration(QVector3D()), m_parentVelocity(QVector3D())
{
  m_animation = QSharedPointer<MTAnimation>::create();
  static const float dt = 1.0f / 120;

  // Position
  for (int ii = 0; ii < 3; ++ii)
  {
    Animation::Property prop;
    {
      prop.setter = [=](float value)
      {
        m_parentAcceleration[ii] = value;
      };
      prop.keyFrames = {};
    }
    m_animation->addProperty(prop);
  }

  // Rotation
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

  // Acceleration
  for (int ii = 0; ii < 3; ++ii)
  {
    Animation::KeyFrame kf;
    {
      kf.time = 0.0f;
      kf.value = 0.0f;
    }
    m_animation->addKeyFrame(ii, kf);
  }

  // Rotation
  QVector3D currRot = m_parent->localRotation();
  for (int jj = 0; jj < 3; ++jj)
  {
    Animation::KeyFrame kf;
    {
      kf.time = 0.0f;
      kf.value = currRot[jj];
    }
    m_animation->addKeyFrame(jj + 3, kf);
  }
}

// ------------------------------------------------------------------------------------------------
void MTAnimatorPlug::update(UpdateInfo infos)
{
  AnimatorPlug::update(infos);

  // Position Update
  QVector3D pos = m_parent->localPosition();
  m_parentVelocity += infos.dt * m_parentAcceleration;
  pos += infos.dt * m_parentVelocity;
  m_parent->setLocalPosition(pos);
}
