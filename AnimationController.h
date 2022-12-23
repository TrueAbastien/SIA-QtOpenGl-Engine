
#pragma once

#include "Component.h"

#include <QElapsedTimer>


class AnimationController final : public QObject
{
  Q_OBJECT;

public:

  AnimationController();
  ~AnimationController();

public:

  void start();
  void pause();
  void stop();

  // In seconds
  float time() const;

public slots:

  void setTime(double value);

private:

  QElapsedTimer* m_timer;
  float m_pausedTime;
};
