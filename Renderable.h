
#pragma once

#include "Component.h"

#include <QOpenGLShaderProgram>
#include <QString>

class Renderable : public Component
{
public:

  Renderable();
  virtual ~Renderable();

  virtual void update(UpdateInfo infos) override;

protected:

  template <typename T>
  void initRenderable(const void* vtxData, int vtxCount,
                      const void* idxData, int idxCount);

  template <typename T>
  void updateRenderable(GLenum mode, int idxCount);

protected:

  void initShaders(const QString& vertexShader, const QString& fragmentShader);

  template <typename T>
  void updateVertices(const void* vtxData, int vtxCount);

private:

  template <typename T> void internalInitRenderable(const void* vtxData, int vtxCount);
  template <> void internalInitRenderable<VertexData_Textured>(const void* vtxData, int vtxCount);
  template <> void internalInitRenderable<VertexData_Colored>(const void* vtxData, int vtxCount);

  template <typename T> void internalUpdateRenderable();
  template <> void internalUpdateRenderable<VertexData_Textured>();
  template <> void internalUpdateRenderable<VertexData_Colored>();

protected:

  QOpenGLShaderProgram m_program;

  QOpenGLBuffer m_vertices;
  QOpenGLBuffer m_indices;
};

// ================================================================================================
template <typename T>
void Renderable::initRenderable(const void* vtxData, int vtxCount, const void* idxData, int idxCount)
{
  Renderable::internalInitRenderable<T>(vtxData, vtxCount);

  // Transfer index data to VBO 1
  m_indices.bind();
  m_indices.allocate(idxData, idxCount * sizeof(GLushort));
}

// ------------------------------------------------------------------------------------------------
template <typename T>
void Renderable::updateRenderable(GLenum mode, int idxCount)
{
  // Tell OpenGL which VBOs to use
  m_vertices.bind();
  m_indices.bind();

  Renderable::internalUpdateRenderable<T>();

  // Draw cube geometry using indices from VBO 1
  glDrawElements(mode, idxCount, GL_UNSIGNED_SHORT, nullptr);
}

// ================================================================================================
template<typename T>
inline void Renderable::updateVertices(const void* vtxData, int vtxCount)
{
  Renderable::internalInitRenderable<T>(vtxData, vtxCount);
}

// ================================================================================================
template<typename T>
inline void Renderable::internalInitRenderable(const void* vtxData, int vtxCount)
{
  throw std::exception("Error, type unrecognized:\n" __FUNCTION__);
}

// ------------------------------------------------------------------------------------------------
template <>
void Renderable::internalInitRenderable<VertexData_Textured>(const void* vtxData, int vtxCount)
{
  // Transfer vertex data to VBO 0
  m_vertices.bind();
  m_vertices.allocate(vtxData, vtxCount * sizeof(VertexData_Textured));
}

// ------------------------------------------------------------------------------------------------
template <>
void Renderable::internalInitRenderable<VertexData_Colored>(const void* vtxData, int vtxCount)
{
  // Transfer vertex data to VBO 0
  m_vertices.bind();
  m_vertices.allocate(vtxData, vtxCount * sizeof(VertexData_Colored));
}

// ================================================================================================
template <typename T>
void Renderable::internalUpdateRenderable()
{
  throw std::exception("Error, type unrecognized:\n" __FUNCTION__);
}

// ------------------------------------------------------------------------------------------------
template <>
void Renderable::internalUpdateRenderable<VertexData_Textured>()
{
  // Tell OpenGL programmable pipeline how to locate vertex position data
  int vertexLocation = m_program.attributeLocation("a_position");
  m_program.enableAttributeArray(vertexLocation);
  m_program.setAttributeBuffer(vertexLocation,
                               GL_FLOAT,
                               offsetof(VertexData_Textured, position),
                               3,
                               sizeof(VertexData_Textured));

  // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
  int texcoordLocation = m_program.attributeLocation("a_texcoord");
  m_program.enableAttributeArray(texcoordLocation);
  m_program.setAttributeBuffer(texcoordLocation,
                               GL_FLOAT,
                               offsetof(VertexData_Textured, texCoord),
                               2,
                               sizeof(VertexData_Textured));
}

// ------------------------------------------------------------------------------------------------
template <>
void Renderable::internalUpdateRenderable<VertexData_Colored>()
{
  // Tell OpenGL programmable pipeline how to locate vertex position data
  int vertexLocation = m_program.attributeLocation("a_position");
  m_program.enableAttributeArray(vertexLocation);
  m_program.setAttributeBuffer(vertexLocation,
                               GL_FLOAT,
                               offsetof(VertexData_Colored, position),
                               3,
                               sizeof(VertexData_Colored));

  // Tell OpenGL programmable pipeline how to locate vertex color data
  int colorLocation = m_program.attributeLocation("a_color");
  m_program.enableAttributeArray(colorLocation);
  m_program.setAttributeBuffer(colorLocation,
                               GL_FLOAT,
                               offsetof(VertexData_Colored, color),
                               3,
                               sizeof(VertexData_Colored));
}
