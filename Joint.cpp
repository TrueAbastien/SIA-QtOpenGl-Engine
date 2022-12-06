
#include "Joint.h"

// ------------------------------------------------------------------------------------------------
void Joint::init()
{
  ColoredRenderable::init();

  m_anchors = {
    VertexData_Colored
    {QVector3D(0, 0, 0), QVector3D(0, 0.8, 0)},
    {QVector3D(0, 0, 0), QVector3D(0.8, 0, 0)}
  };

  GLushort indices[] = {0,1};

  ColoredRenderable::initRenderable(m_anchors.data(), 2, indices, 2);
}

// ------------------------------------------------------------------------------------------------
void Joint::update(UpdateInfo infos)
{
  ColoredRenderable::update(infos);

  // Change First Position
  m_anchors[0].position = m_localToParent.inverted() * QVector3D(0, 0, 0);
  Renderable::updateVertices<VertexData_Colored>(m_anchors.data(), 2);

  ColoredRenderable::updateRenderable(GL_LINES, 2);
}
