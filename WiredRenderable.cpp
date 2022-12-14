#include "WiredRenderable.h"

// ------------------------------------------------------------------------------------------------
void WiredRenderable::init()
{
  initShaders();
}

// ------------------------------------------------------------------------------------------------
void WiredRenderable::initRenderable(const void* vtxData, int vtxCount, const void* idxData, int idxCount)
{
  Renderable::initRenderable<VertexData_Colored>(vtxData, vtxCount, idxData, idxCount);
}

// ------------------------------------------------------------------------------------------------
void WiredRenderable::updateRenderable(GLenum mode, int idxCount)
{
  Renderable::updateRenderable<VertexData_Colored>(mode, idxCount);
}

// ------------------------------------------------------------------------------------------------
void WiredRenderable::initShaders()
{
  Renderable::initShaders(":/shaders/vshaderW.glsl", ":/shaders/fshaderW.glsl");
}
