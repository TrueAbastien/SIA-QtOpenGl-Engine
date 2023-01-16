#pragma once

#include "Animation.h"

#include <QQuaternion>

class MTAnimation final : public Animation<QVector3D>
{
public:

  MTAnimation() = default;

protected:

  QVector3D interpolateValue(const KeyFrame* a, const KeyFrame* b, float time) const override;
};