#pragma once

#include "AnimatorPlug.h"
#include "MTAnimationData.h"

class MTAnimatorPlug : public AnimatorPlugBase<MTAnimationData::JointFrame>
{
public:

  MTAnimatorPlug();

public:

  void init() override;
};