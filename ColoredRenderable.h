
#pragma once

#include "Renderable.h"

class ColoredRenderable : public Renderable
{
public:

  virtual void init() override;

protected:

  void initRenderable(const void* vtxData, int vtxCount,
                      const void* idxData, int idxCount);
  void updateRenderable(GLenum mode, int idxCount);

protected:

  void initShaders();
};
