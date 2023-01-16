#include "MTAnimation.h"

// ------------------------------------------------------------------------------------------------
QVector3D MTAnimation::interpolateValue(const KeyFrame* a, const KeyFrame* b, float) const
{
  if (a == nullptr && b == nullptr)
    return QVector3D();

  else if (a == nullptr)
    return b->value;

  return a->value;
}
