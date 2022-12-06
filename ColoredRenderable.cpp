#include "ColoredRenderable.h"

// ------------------------------------------------------------------------------------------------
void ColoredRenderable::init()
{
  initShaders();
}

// ------------------------------------------------------------------------------------------------
void ColoredRenderable::initRenderable(const void* vtxData, int vtxCount, const void* idxData, int idxCount)
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
