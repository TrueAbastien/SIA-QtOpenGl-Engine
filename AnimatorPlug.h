
#pragma once

#include "Component.h"
#include "Animation.h"

class AnimatorPlug : public Component
{
public:

  enum PropertyType
  {
    PositionX = 0,
    PositionY,
    PositionZ,

    RotationX,
    RotationY,
    RotationZ,

    __COUNT__
  };

public:

  AnimatorPlug();

public:

  void init() override;
  void update(UpdateInfo infos) override;

  void addKeyFrame(const PropertyType& type, float time, float value);

protected:

  QSharedPointer<Animation> m_animation;
};
