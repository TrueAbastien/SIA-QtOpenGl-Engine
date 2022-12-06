
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

  void initShaders();
  void initTexture();

protected:

  QOpenGLTexture* m_texture = nullptr;
};
