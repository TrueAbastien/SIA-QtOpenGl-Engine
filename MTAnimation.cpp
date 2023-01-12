#include "MTAnimation.h"

// ------------------------------------------------------------------------------------------------
QQuaternion MTAnimation::interpolateValue(const KeyFrame* a, const KeyFrame* b, float) const
{
  if (a == nullptr && b == nullptr)
    return QQuaternion();

  else if (a == nullptr)
    return b->value;

  return a->value;
}
