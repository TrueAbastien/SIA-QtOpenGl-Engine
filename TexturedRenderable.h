
#pragma once

#include "Renderable.h"

#include <QOpenGLTexture>

class TexturedRenderable : public Renderable
{
public:

  TexturedRenderable(const QString& texFile);
  virtual ~TexturedRenderable();

  virtual void init() override;
  virtual void update(UpdateInfo infos) override;

protected:

  void initRenderable(VertexData_Textured* vtxData, int vtxCount,
                      const GLushort* idxData, int idxCount);
  void updateRenderable(GLenum mode, int idxCount);

protected:

  void initShaders();
  void initTexture();

protected:

  QOpenGLTexture* m_texture = nullptr;
  QString m_textureFile;
};
