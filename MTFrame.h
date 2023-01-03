#pragma once

#include "WiredRenderable.h"

class MTFrame : public WiredRenderable
{
public:

  void init() override;
  void update(UpdateInfo infos) override;
};