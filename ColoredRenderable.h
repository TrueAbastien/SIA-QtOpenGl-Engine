
#pragma once

#include "Renderable.h"

class ColoredRenderable : public Renderable
{
public:

  virtual void init() override;

protected:

  void initShaders();
};
