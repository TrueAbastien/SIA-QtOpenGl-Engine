
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

  // Vertex Uniforms
  m_program.setUniformValue("model_matrix", infos.parentToWorld * m_localToParent);
  m_program.setUniformValue("cam_position", infos.cameraPosition);

  // Fragment Uniforms
  m_program.setUniformValue("texture", 0);
  m_program.setUniformValue("ka", infos.material.ka);
  m_program.setUniformValue("kd", infos.material.kd);
  m_program.setUniformValue("ks", infos.material.ks);
  m_program.setUniformValue("shininess", infos.material.shininess);
}

// ------------------------------------------------------------------------------------------------
void TexturedRenderable::initRenderable(VertexData_Textured* vtxData, int vtxCount, const GLushort* idxData, int idxCount)
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
