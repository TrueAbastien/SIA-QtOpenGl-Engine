#include "FactoryFloor.h"

#include <array>

const float w = 10.0f;
const int n = 10;

// ------------------------------------------------------------------------------------------------
void FactoryFloor::init()
{
  WiredRenderable::init();

  float s = w / (n - 1);
  float o = -w / 2;

  std::array<VertexData_Colored, n * n> vertices;
  std::array<GLushort, n << 2> indices;

  // Vertices
  for (int y = 0; y < n; ++y)
  {
    for (int x = 0; x < n; ++x)
    {
      vertices[y * n + x] =
      {
        QVector3D(o + s * x, o + s * y, 0.0f),
        QVector3D(0.5f, 0.5f, 0.2f)
      };
    }
  }

  // Indices
  const int h = (n - 1);
  const int v = h * n;
  // -- Vertical
  for (int x = 0; x < n; ++x)
  {
    int index = (x << 1);
    indices[index] = x;
    indices[index + 1] = v + x;
  }
  // -- Horizontal
  for (int y = 0; y < n; ++y)
  {
    int index = ((y + n) << 1);
    indices[index] = y * n;
    indices[index + 1] = y * n + h;
  }

  WiredRenderable::initRenderable(vertices.data(), n * n, indices.data(), n << 2);
}

// ------------------------------------------------------------------------------------------------
void FactoryFloor::update(UpdateInfo infos)
{
  WiredRenderable::update(infos);

  WiredRenderable::updateRenderable(GL_LINES, n << 2);
}
