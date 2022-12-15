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

  m_program.setUniformValue("model_matrix", infos.parentToWorld * m_localToParent);
  m_program.setUniformValue("cam_position", infos.cameraPosition);
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
