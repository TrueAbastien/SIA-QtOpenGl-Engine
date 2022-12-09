
#pragma once

#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>

#define M_PI 3.14159265358979323846f  /* pi */
#define ROT_EPS 0.001f

struct VertexData_Textured
{
  QVector3D position;
  QVector2D texCoord;
};

struct VertexData_Colored
{
  QVector3D position;
  QVector3D color;
};

struct UpdateInfo
{
  float dt;
  float animationTime;
  QMatrix4x4 screenToParent;
};
