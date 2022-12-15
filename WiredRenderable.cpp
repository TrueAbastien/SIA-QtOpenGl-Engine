#include "WiredRenderable.h"

// ------------------------------------------------------------------------------------------------
void WiredRenderable::init()
{
  initShaders();
}

// ------------------------------------------------------------------------------------------------
void WiredRenderable::initRenderable(VertexData_Wired* vtxData, int vtxCount, const GLushort* idxData, int idxCount)
{
  Renderable::initRenderable<VertexData_Wired>(vtxData, vtxCount, idxData, idxCount);
}

// ------------------------------------------------------------------------------------------------
void WiredRenderable::updateRenderable(GLenum mode, int idxCount)
{
  Renderable::updateRenderable<VertexData_Wired>(mode, idxCount);
}

// ------------------------------------------------------------------------------------------------
void WiredRenderable::initShaders()
{
  Renderable::initShaders(":/shaders/vshaderW.glsl", ":/shaders/fshaderW.glsl");
}
