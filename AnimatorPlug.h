
#pragma once

#include "AnimatorPlugBase.h"
#include "Animation.h"

class AnimatorPlug : public AnimatorPlugBase<QVector3D>
{
public:

  AnimatorPlug();

public:

  void init() override;
};