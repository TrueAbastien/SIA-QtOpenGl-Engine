
#pragma once

#include "Renderable.h"

class WiredRenderable : public Renderable
{
public:

  virtual void init() override;

protected:

  void initRenderable(VertexData_Wired* vtxData, int vtxCount,
                      const GLushort* idxData, int idxCount);
  void updateRenderable(GLenum mode, int idxCount);

protected:

  void initShaders();
};
