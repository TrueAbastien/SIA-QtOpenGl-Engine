#pragma once

#include "AnimatorPlug.h"

class MTAnimatorPlug : public AnimatorPlugBase<QQuaternion>
{
public:

  MTAnimatorPlug();

public:

  void init() override;
};