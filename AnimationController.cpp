#include "AnimationController.h"

// ------------------------------------------------------------------------------------------------
AnimationController::AnimationController()
  : m_timer(nullptr), m_pausedTime(0.0f)
{
}

// ------------------------------------------------------------------------------------------------
AnimationController::~AnimationController()
{
  if (m_timer != nullptr)
  {
    delete m_timer;
  }
}

// ------------------------------------------------------------------------------------------------
void AnimationController::start()
{
  if (m_timer == nullptr)
  {
    m_timer = new QElapsedTimer();
  }

  m_pausedTime = 0.0f;
  m_timer->start();
}

// ------------------------------------------------------------------------------------------------
void AnimationController::pause()
{
  if (m_timer == nullptr)
  {
    return;
  }

  if (m_timer->isValid())
  {
    m_pausedTime = time();
    m_timer->invalidate();
  }
  else
  {
    m_timer->start();
  }
}

// ------------------------------------------------------------------------------------------------
void AnimationController::stop()
{
  if (m_timer == nullptr)
  {
    return;
  }

  m_timer->invalidate();
  m_pausedTime = 0.0f;
}

// ------------------------------------------------------------------------------------------------
float AnimationController::time() const
{
  if (m_timer == nullptr)
  {
    return 0.0f;
  }

  if (!m_timer->isValid())
  {
    return m_pausedTime;
  }

  return m_pausedTime + ((float) m_timer->elapsed()) / 1'000.0f;
}

// ------------------------------------------------------------------------------------------------
void AnimationController::setTime(double value)
{
  float t = this->time();
  m_pausedTime += ((float) value) - t;
}
