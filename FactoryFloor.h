
#pragma once

#include "ColoredRenderable.h"

class FactoryFloor final : public ColoredRenderable
{
public:

  void init() override;
  void update(UpdateInfo infos) override;
};
