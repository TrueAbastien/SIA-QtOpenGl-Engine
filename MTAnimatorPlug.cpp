#include "MTAnimatorPlug.h"

#include "MTAnimation.h"

// ------------------------------------------------------------------------------------------------
MTAnimatorPlug::MTAnimatorPlug()
{
  m_animation = QSharedPointer<MTAnimation>::create();

  // Animation Data
  {
    MTAnimation::Property prop;
    {
      prop.setter = [&](MTAnimationData::JointFrame value)
      {
        const auto method = [=](QVector3D, QVector3D)
        {
          QMatrix4x4 p;
          p.translate(value.worldOffset);
          return p * value.localToRoot;
        };
        m_parent->setMatrixConstruct(method);
      };
      prop.keyFrames = {};
    }
    m_animation->addProperty(prop);
  }
}
