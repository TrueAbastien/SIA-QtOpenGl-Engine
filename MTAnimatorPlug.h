#pragma once

#include "AnimatorPlug.h"

class MTAnimatorPlug : public AnimatorPlugBase<QVector3D>
{
public:

  MTAnimatorPlug();

public:

  void init() override;

private:

  QVector3D m_originalRotation;
};