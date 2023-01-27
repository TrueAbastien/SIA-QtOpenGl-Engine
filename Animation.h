
#pragma once

#include "AnimationBase.h"

#include <QVector3D>

class Animation : public AnimationBase<QVector3D>
{
public:

  Animation() = default;

protected:

  QVector3D interpolateValue(const KeyFrame* a, const KeyFrame* b, float time) const override;
};
