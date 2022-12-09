
#pragma once

#include "Component.h"

#include <QElapsedTimer>


class AnimationController final
{
public:

  AnimationController();
  ~AnimationController();

public:

  void start();
  void pause();
  void stop();

  // In seconds
  float time() const;

private:

  QElapsedTimer* m_timer;
  float m_pausedTime;
};
