
#pragma once

#include "Renderable.h"

#include <QOpenGLTexture>

class TexturedRenderable : public Renderable
{
public:

  TexturedRenderable();
  virtual ~TexturedRenderable();

  virtual void init() override;
  virtual void update(UpdateInfo infos) override;

protected:

  void initRenderable(const void* vtxData, int vtxCount,
                      const void* idxData, int idxCount);
  void updateRenderable(GLenum mode, int idxCount);

protected:

  void initShaders();
  void initTexture();

protected:

  QOpenGLTexture* m_texture = nullptr;
};
