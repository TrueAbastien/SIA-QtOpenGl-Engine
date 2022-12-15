
#pragma once

#include "Renderable.h"

class ColoredRenderable : public Renderable
{
public:

  virtual void init() override;
  virtual void update(UpdateInfo infos) override;

protected:

  void initRenderable(VertexData_Colored* vtxData, int vtxCount,
                      const GLushort* idxData, int idxCount);
  void updateRenderable(GLenum mode, int idxCount);

protected:

  void initShaders();
};
