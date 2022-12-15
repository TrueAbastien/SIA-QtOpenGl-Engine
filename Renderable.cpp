#include "Renderable.h"

#include <QOpenGLShader>

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
#ifdef _DEBUG
  {
    // Compile the Shaders
    // -- Vertex
    {
      QOpenGLShader vShader(QOpenGLShader::Vertex);
      if (!vShader.compileSourceFile(vertexShader))
      {
        CustomDebug::log(vShader.log().toStdString());
      }
    }
    // -- Fragment
    {
      QOpenGLShader fShader(QOpenGLShader::Fragment);
      if (!fShader.compileSourceFile(fragmentShader))
      {
        CustomDebug::log(fShader.log().toStdString());
      }
    }
  }
#endif

  // Compile vertex shader
  m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShader);

  // Compile fragment shader
  m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShader);

  // Link shader pipeline
  m_program.link();

#ifdef _DEBUG
  {
    // Test the shader
    QString r = m_program.log();
    if (!r.isEmpty()) CustomDebug::log(r.toStdString());
  }
#endif
}
