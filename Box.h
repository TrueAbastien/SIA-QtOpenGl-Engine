
#pragma once

#include "ColoredRenderable.h"

class Box : public ColoredRenderable
{
public:

  Box(QVector3D size = QVector3D(1, 1, 1));

public:

  void init() override;
  void update(UpdateInfo infos) override;

private:

  QVector3D m_size;
};