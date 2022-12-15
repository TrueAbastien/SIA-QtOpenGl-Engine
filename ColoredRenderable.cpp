#include "ColoredRenderable.h"

// ------------------------------------------------------------------------------------------------
void ColoredRenderable::init()
{
  initShaders();
}

// ------------------------------------------------------------------------------------------------
void ColoredRenderable::update(UpdateInfo infos)
{
  Renderable::update(infos);

  // Vertex Uniforms
  m_program.setUniformValue("model_matrix", infos.parentToWorld * m_localToParent);

  // Fragment Uniforms
  m_program.setUniformValue("ka", infos.material.ka);
  m_program.setUniformValue("kd", infos.material.kd);
  m_program.setUniformValue("ks", infos.material.ks);
  m_program.setUniformValue("shininess", infos.material.shininess);
  m_program.setUniformValue("cam_dir", infos.cameraDirection);
}

// ------------------------------------------------------------------------------------------------
void ColoredRenderable::initRenderable(VertexData_Colored* vtxData, int vtxCount, const GLushort* idxData, int idxCount)
{
  Renderable::initRenderable<VertexData_Colored>(vtxData, vtxCount, idxData, idxCount);
}

// ------------------------------------------------------------------------------------------------
void ColoredRenderable::updateRenderable(GLenum mode, int idxCount)
{
  Renderable::updateRenderable<VertexData_Colored>(mode, idxCount);
}

// ------------------------------------------------------------------------------------------------
void ColoredRenderable::initShaders()
{
  Renderable::initShaders(":/shaders/vshaderC.glsl", ":/shaders/fshaderC.glsl");
}
