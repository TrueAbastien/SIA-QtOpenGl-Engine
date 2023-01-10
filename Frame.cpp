#include "Frame.h"

// ------------------------------------------------------------------------------------------------
void Frame::init()
{
  WiredRenderable::init();

  VertexData_Wired vertices[6] =
  {
    // X
    {
      QVector3D(0, 0, 0),
      QVector3D(1, 0, 0)
    },
    {
      QVector3D(1, 0, 0),
      QVector3D(1, 0, 0)
    },

    // Y
    {
      QVector3D(0, 0, 0),
      QVector3D(0, 1, 0)
    },
    {
      QVector3D(0, 1, 0),
      QVector3D(0, 1, 0)
    },

    // Z
    {
      QVector3D(0, 0, 0),
      QVector3D(0, 0, 1)
    },
    {
      QVector3D(0, 0, 1),
      QVector3D(0, 0, 1)
    }
  };

  GLushort indices[6] =
  {
    0, 1, 2, 3, 4, 5
  };

  WiredRenderable::initRenderable(vertices, 6, indices, 6);
}

// ------------------------------------------------------------------------------------------------
void Frame::update(UpdateInfo infos)
{
  WiredRenderable::update(infos);

  WiredRenderable::updateRenderable(GL_LINES, 6);
}
