#pragma once

#include "Animation.h"

#include <QQuaternion>

class MTAnimation final : public Animation<QQuaternion>
{
public:

  MTAnimation() = default;

protected:

  QQuaternion interpolateValue(const KeyFrame* a, const KeyFrame* b, float time) const override;
};