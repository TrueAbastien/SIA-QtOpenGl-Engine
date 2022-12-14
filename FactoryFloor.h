
#pragma once

#include "WiredRenderable.h"

class FactoryFloor final : public WiredRenderable
{
public:

  void init() override;
  void update(UpdateInfo infos) override;
};
