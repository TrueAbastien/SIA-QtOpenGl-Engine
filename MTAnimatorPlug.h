#pragma once

#include "AnimatorPlug.h"

class MTAnimatorPlug : public AnimatorPlug
{
public:

  MTAnimatorPlug();

public:

  void init() override;
  void update(UpdateInfo infos) override;

protected:

  QVector3D m_parentAcceleration;
  QVector3D m_parentVelocity;
};