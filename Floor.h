
#pragma once

#include "TexturedRenderable.h"

class Floor : public TexturedRenderable
{
public:

  Floor();

public:

  void init() override;
  void update(UpdateInfo infos) override;
};