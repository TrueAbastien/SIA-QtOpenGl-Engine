#pragma once

#include "WiredRenderable.h"

class Frame : public WiredRenderable
{
public:

  void init() override;
  void update(UpdateInfo infos) override;
};