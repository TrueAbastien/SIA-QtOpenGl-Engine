
#pragma once

#include "TexturedRenderable.h"

class Cube final : public TexturedRenderable
{
public:

  Cube();

public:

  void init() override;
  void update(UpdateInfo infos) override;
};
