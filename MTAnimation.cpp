#include "MTAnimation.h"

// ------------------------------------------------------------------------------------------------
MTAnimationData::JointFrame MTAnimation::interpolateValue(const KeyFrame* a, const KeyFrame* b, float) const
{
  if (a == nullptr && b == nullptr)
    return MTAnimationData::JointFrame();

  else if (a == nullptr)
    return b->value;

  return a->value;
}
