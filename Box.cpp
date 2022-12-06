#include "Box.h"

// ------------------------------------------------------------------------------------------------
Box::Box(QVector3D size) :
  m_size(size)
{
}

// ------------------------------------------------------------------------------------------------
void Box::init()
{
  ColoredRenderable::init();

  QVector3D h = m_size * 0.5f;

  //    C ----- G      Y
  //   /|      /|     /
  // A ----- E  |   O ----- X
  // |  |    |  |   |
  // |  D ---|- H   |
  // | /     | /    |
  // B ----- F      Z

  VertexData_Colored vertices[] = {
    {QVector3D(-h.x(), -h.y(), -h.z()), QVector3D(0, 0, 0)}, // A (0)
    {QVector3D(-h.x(), -h.y(), +h.z()), QVector3D(0, 0, 1)}, // B (1)
    {QVector3D(-h.x(), +h.y(), -h.z()), QVector3D(0, 1, 0)}, // C (2)
    {QVector3D(-h.x(), +h.y(), +h.z()), QVector3D(0, 1, 1)}, // D (3)
    {QVector3D(+h.x(), -h.y(), -h.z()), QVector3D(1, 0, 0)}, // E (4)
    {QVector3D(+h.x(), -h.y(), +h.z()), QVector3D(1, 0, 1)}, // F (5)
    {QVector3D(+h.x(), +h.y(), -h.z()), QVector3D(1, 1, 0)}, // G (6)
    {QVector3D(+h.x(), +h.y(), +h.z()), QVector3D(1, 1, 1)}  // H (7)
  };

  GLushort indices[] = {
    0, 1, 3, 0, 3, 2, // ABDC (0132)
    0, 2, 6, 0, 6, 4, // ACGE (0264)
    0, 4, 5, 0, 5, 1, // AEFB (0451)
    7, 3, 1, 7, 1, 5, // HDBF (7315)
    7, 5, 4, 7, 4, 6, // HFEG (7546)
    7, 6, 2, 7, 2, 3  // HGCD (7623)
  };

  ColoredRenderable::initRenderable(vertices, 8, indices, 36);
}

// ------------------------------------------------------------------------------------------------
void Box::update(UpdateInfo infos)
{
  ColoredRenderable::update(infos);

  ColoredRenderable::updateRenderable(GL_TRIANGLES, 36);
}
