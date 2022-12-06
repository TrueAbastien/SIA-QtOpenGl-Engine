
#include "TexturedRenderable.h"

// ------------------------------------------------------------------------------------------------
TexturedRenderable::TexturedRenderable()
{
  // TODO
}

// ------------------------------------------------------------------------------------------------
TexturedRenderable::~TexturedRenderable()
{
  delete m_texture;
}

// ------------------------------------------------------------------------------------------------
void TexturedRenderable::init()
{
  Renderable::init();

  initTexture();
}

// ------------------------------------------------------------------------------------------------
void TexturedRenderable::update(UpdateInfo infos)
{
  m_texture->bind();

  Renderable::update(infos);

  m_program.setUniformValue("texture", 0);
}

// ------------------------------------------------------------------------------------------------
void TexturedRenderable::initTexture()
{
  // Load cube.png image
  m_texture = new QOpenGLTexture(QImage(":/textures/cube.png").mirrored());

  // Set nearest filtering mode for texture minification
  m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

  // Set bilinear filtering mode for texture magnification
  m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

  // Wrap texture coordinates by repeating
  // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
  m_texture->setWrapMode(QOpenGLTexture::Repeat);
}
