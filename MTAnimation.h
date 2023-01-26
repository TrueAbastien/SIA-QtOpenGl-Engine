#pragma once

#include "Animation.h"
#include "MTAnimationData.h"

#include <QQuaternion>

class MTAnimation final : public Animation<MTAnimationData::JointFrame>
{
public:

  MTAnimation() = default;

protected:

  MTAnimationData::JointFrame interpolateValue(const KeyFrame* a, const KeyFrame* b, float time) const override;
};