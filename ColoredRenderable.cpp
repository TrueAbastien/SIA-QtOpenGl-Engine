#include "ColoredRenderable.h"

// ------------------------------------------------------------------------------------------------
void ColoredRenderable::init()
{
  initShaders();
}

// ------------------------------------------------------------------------------------------------
void ColoredRenderable::initShaders()
{
  Renderable::initShaders(":/shaders/vshaderC.glsl", ":/shaders/fshaderC.glsl");
}
