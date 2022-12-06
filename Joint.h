
#pragma once

#include "ColoredRenderable.h"

#include <array>

class Joint : public ColoredRenderable
{
public:

  void init() override;
  void update(UpdateInfo infos) override;

private:

  std::array<VertexData_Colored, 2> m_anchors;
};