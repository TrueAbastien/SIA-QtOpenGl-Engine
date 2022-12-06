
#pragma once

#include "Component.h"

#include <QOpenGLShaderProgram>

class Renderable : public Component
{
public:

  Renderable();
  virtual ~Renderable();

  virtual void init() override;
  virtual void update(UpdateInfo infos) override;

protected:

  void initRenderable(const void* vtxData, int vtxCount,
                      const void* idxData, int idxCount);
  void updateRenderable(GLenum mode, int idxCount);

private:

  void initShaders();

protected:

  QOpenGLShaderProgram m_program;

  QOpenGLBuffer m_vertices;
  QOpenGLBuffer m_indices;
};
