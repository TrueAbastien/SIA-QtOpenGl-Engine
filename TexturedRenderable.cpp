
#include "TexturedRenderable.h"

// ------------------------------------------------------------------------------------------------
TexturedRenderable::TexturedRenderable(const QString& texFile) :
  m_textureFile(texFile)
{
}

// ------------------------------------------------------------------------------------------------
TexturedRenderable::~TexturedRenderable()
{
  delete m_texture;
}

// ------------------------------------------------------------------------------------------------
void TexturedRenderable::init()
{
  initShaders();
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
void TexturedRenderable::initRenderable(const void* vtxData, int vtxCount, const void* idxData, int idxCount)
{
  Renderable::initRenderable<VertexData_Textured>(vtxData, vtxCount, idxData, idxCount);
}

// ------------------------------------------------------------------------------------------------
void TexturedRenderable::updateRenderable(GLenum mode, int idxCount)
{
  Renderable::updateRenderable<VertexData_Textured>(mode, idxCount);
}

// ------------------------------------------------------------------------------------------------
void TexturedRenderable::initShaders()
{
  Renderable::initShaders(":/shaders/vshaderT.glsl", ":/shaders/fshaderT.glsl");
}

// ------------------------------------------------------------------------------------------------
void TexturedRenderable::initTexture()
{
  // Load cube.png image
  m_texture = new QOpenGLTexture(QImage(m_textureFile).mirrored());

  // Set nearest filtering mode for texture minification
  m_texture->setMinificationFilter(QOpenGLTexture::Nearest);

  // Set bilinear filtering mode for texture magnification
  m_texture->setMagnificationFilter(QOpenGLTexture::Linear);

  // Wrap texture coordinates by repeating
  // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
  m_texture->setWrapMode(QOpenGLTexture::Repeat);
}
