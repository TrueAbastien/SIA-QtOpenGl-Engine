#include "Animation.h"

// ------------------------------------------------------------------------------------------------
QVector3D Animation::interpolateValue(const KeyFrame* a, const KeyFrame* b, float time) const
{
  if (a == nullptr && b == nullptr)
    return QVector3D();

  else if (a == nullptr)
    return b->value;

  else if (b == nullptr)
    return a->value;

  float k = (time - a->time) / (b->time - a->time);
  QVector3D dist = (b->value - a->value) * k;
  return dist + a->value;
}

