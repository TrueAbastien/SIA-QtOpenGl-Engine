
#include "Floor.h"

// ------------------------------------------------------------------------------------------------
Floor::Floor() :
  TexturedRenderable(":/textures/floor.png")
{
}

// ------------------------------------------------------------------------------------------------
void Floor::init()
{
  TexturedRenderable::init();

  const float w = 5.0f;

  VertexData_Textured vertices[] = {
    {QVector3D(-w, -w, 0.0f), QVector2D(0.0f, 0.0f)},
    {QVector3D(-w, +w, 0.0f), QVector2D(0.0f, 1.0f)},
    {QVector3D(+w, -w, 0.0f), QVector2D(1.0f, 0.0f)},
    {QVector3D(+w, +w, 0.0f), QVector2D(1.0f, 1.0f)},
  };

  GLushort indices[] = {0,3,1,0,2,3};

  TexturedRenderable::initRenderable(vertices, 4, indices, 6);
}

// ------------------------------------------------------------------------------------------------
void Floor::update(UpdateInfo infos)
{
  TexturedRenderable::update(infos);

  TexturedRenderable::updateRenderable(GL_TRIANGLES, 6);
}
