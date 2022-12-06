
#pragma once

#include "ColoredRenderable.h"

#include <array>

class Joint : public Component
{
public:

  void init() override;
  void update(UpdateInfo infos) override;

  void addChildren(const Pointer& child) override;
};