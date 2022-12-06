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
void Renderable::init()
{
  initShaders();
}

// ------------------------------------------------------------------------------------------------
void Renderable::update(UpdateInfo infos)
{
  m_program.bind();

  // Set modelview-projection matrix
  m_program.setUniformValue("mvp_matrix", infos.screenToParent * m_localToParent);
}

// ------------------------------------------------------------------------------------------------
void Renderable::initRenderable(const void* vtxData, int vtxCount, const void* idxData, int idxCount)
{
  // Transfer vertex data to VBO 0
  m_vertices.bind();
  m_vertices.allocate(vtxData, vtxCount * sizeof(VertexData));

  // Transfer index data to VBO 1
  m_indices.bind();
  m_indices.allocate(idxData, idxCount * sizeof(GLushort));
}

// ------------------------------------------------------------------------------------------------
void Renderable::updateRenderable(GLenum mode, int idxCount)
{
  // Tell OpenGL which VBOs to use
  m_vertices.bind();
  m_indices.bind();

  // Tell OpenGL programmable pipeline how to locate vertex position data
  int vertexLocation = m_program.attributeLocation("a_position");
  m_program.enableAttributeArray(vertexLocation);
  m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, offsetof(VertexData, position), 3, sizeof(VertexData));

  // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
  int texcoordLocation = m_program.attributeLocation("a_texcoord");
  m_program.enableAttributeArray(texcoordLocation);
  m_program.setAttributeBuffer(texcoordLocation, GL_FLOAT, offsetof(VertexData, texCoord), 2, sizeof(VertexData));

  // Draw cube geometry using indices from VBO 1
  glDrawElements(mode, idxCount, GL_UNSIGNED_SHORT, nullptr);
}

// ------------------------------------------------------------------------------------------------
void Renderable::initShaders()
{
  // Compile vertex shader
  m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vshader.glsl");

  // Compile fragment shader
  m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fshader.glsl");

  // Link shader pipeline
  m_program.link();

  // Bind shader pipeline for use
  //m_program.bind();
}
