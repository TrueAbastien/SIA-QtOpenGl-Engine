
#pragma once

#include "AnimatorPlugBase.h"

class AnimatorPlug : public AnimatorPlugBase<QVector3D>
{
public:

  using AnimationType = Animation<QVector3D>;

public:

  AnimatorPlug();

public:

  void init() override;
};