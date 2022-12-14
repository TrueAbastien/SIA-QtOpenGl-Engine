#include "Renderable.h"

// ------------------------------------------------------------------------------------------------
Renderable::Renderable() :
  m_indices(QOpenGLBuffer::IndexBuffer)
{
  initializeOpenGLFunctions();

  // Generate 2 VBOs
  m_vertices.create();
  m_indices.create();
}

// ------------------------------------------------------------------------------------------------
Renderable::~Renderable()
{
  m_vertices.destroy();
  m_indices.destroy();
}

// ------------------------------------------------------------------------------------------------
void Renderable::update(UpdateInfo infos)
{
  m_program.bind();

  // Set modelview-projection matrix
  m_program.setUniformValue("mvp_matrix", infos.parentToScreen * m_localToParent);
}

// ------------------------------------------------------------------------------------------------
void Renderable::initShaders(const QString& vertexShader, const QString& fragmentShader)
{
  // Compile vertex shader
  m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShader);

  // Compile fragment shader
  m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShader);

  // Link shader pipeline
  m_program.link();

  // Bind shader pipeline for use
  //m_program.bind();
}
