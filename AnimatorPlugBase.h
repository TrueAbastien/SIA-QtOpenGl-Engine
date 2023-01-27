
#pragma once

#include "Component.h"
#include "AnimationBase.h"

template <typename T>
class AnimatorPlugBase : public Component
{
protected:

  AnimatorPlugBase() = default;

public:

  void update(UpdateInfo infos) override;

  void addKeyFrame(int typeIndex, float time, T value);

  QSharedPointer<AnimationBase<T>> animation() const;

protected:

  QSharedPointer<AnimationBase<T>> m_animation;
};

// ------------------------------------------------------------------------------------------------
template <typename T>
void AnimatorPlugBase<T>::update(UpdateInfo infos)
{
  m_animation->updateProperties(infos.animationTime);
}

// ------------------------------------------------------------------------------------------------
template <typename T>
void AnimatorPlugBase<T>::addKeyFrame(int typeIndex, float time, T value)
{
  AnimationBase<T>::KeyFrame kf;
  {
    kf.time = time;
    kf.value = value;
  }
  m_animation->addKeyFrame(typeIndex, kf);
}

// ------------------------------------------------------------------------------------------------
template<typename T>
inline QSharedPointer<AnimationBase<T>> AnimatorPlugBase<T>::animation() const
{
  return m_animation;
}
