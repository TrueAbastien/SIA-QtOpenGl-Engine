#pragma once

#include "Animation.h"

class MTAnimation final : public Animation
{
public:

  MTAnimation() = default;

protected:

  float interpolateValue(const KeyFrame* a, const KeyFrame* b, float time) const override;
};