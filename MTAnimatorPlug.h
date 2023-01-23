#pragma once

#include "AnimatorPlug.h"

class MTAnimatorPlug : public AnimatorPlugBase<QVector3D>
{
public:

  MTAnimatorPlug();

public:

  void init() override;

  QVector3D originalRotation() const;
  QVector3D acceleration() const;

private:

  QVector3D m_originalRotation;
  QVector3D m_acceleration;
};