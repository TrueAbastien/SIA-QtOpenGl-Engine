
#pragma once

#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>

#include <functional>
#include <string>

#define M_PI 3.14159265358979323846f  /* pi */
#define ROT_EPS 0.001f

#define RAD2DEG (180.0f / M_PI)

#ifdef _DEBUG
struct CustomDebug final
{
  using LogMethod = std::function<void(const std::string&)>;
  static LogMethod log;
};
#endif

struct VertexData_Textured
{
  QVector3D position;
  QVector2D texCoord;
  QVector3D normal;
};

struct VertexData_Colored
{
  QVector3D position;
  QVector3D color;
  QVector3D normal;
};

struct VertexData_Wired
{
  QVector3D position;
  QVector3D color;
};

struct GlobalMaterial
{
  float ka;
  float kd;
  float ks;
  float shininess;
};

struct UpdateInfo
{
  float dt;
  float animationTime;
  QMatrix4x4 parentToScreen;
  QMatrix4x4 parentToWorld;
  QVector3D cameraDirection;
  GlobalMaterial material;
};

enum LogType
{
  DEBUG,
  INFO,
  WARNING,
  ERROR_,
  CRITICAL
};

using LogMethod = std::function<void(LogType, const std::string&)>;

using UID = unsigned long long;
