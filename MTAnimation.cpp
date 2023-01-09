#include "MTAnimation.h"

// ------------------------------------------------------------------------------------------------
float MTAnimation::interpolateValue(const KeyFrame* a, const KeyFrame* b, float) const
{
  if (a == nullptr && b == nullptr)
    return 0.0f;

  else if (a == nullptr)
    return b->value;

  return a->value;
}
