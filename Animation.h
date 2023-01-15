
#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <functional>

template <typename T>
class Animation
{
public:

  using PropertyCallback = std::function<void(T)>;

  struct KeyFrame
  {
    float time;
    T value;
  };
  struct Property
  {
    PropertyCallback setter;
    std::vector<KeyFrame> keyFrames;
  };

public:

  Animation() = default;

public:

  void addProperty(const Property& prop);
  Property getProperty(int index) const;
  Property& property(int index);
  void clearProperties();

  void addKeyFrame(int index, const KeyFrame& kf);

  void updateProperties(float time);

protected:

  virtual T interpolateValue(const KeyFrame* a, const KeyFrame* b, float time) const;

protected:

  std::vector<Property> m_properties;
};

// ------------------------------------------------------------------------------------------------
template <typename T>
inline void Animation<T>::addProperty(const Property& prop)
{
  m_properties.push_back(prop);
}

// ------------------------------------------------------------------------------------------------
template <typename T>
inline typename Animation<T>::Property Animation<T>::getProperty(int index) const
{
  if (index < 0 || index >= m_properties.size())
    return Property();

  return m_properties.at(index);
}

// ------------------------------------------------------------------------------------------------
template<typename T>
inline typename Animation<T>::Property& Animation<T>::property(int index)
{
  if (index < 0 || index >= m_properties.size())
    return m_properties.front();

  return m_properties[index];
}

// ------------------------------------------------------------------------------------------------
template <typename T>
inline void Animation<T>::clearProperties()
{
  m_properties.clear();
}

// ------------------------------------------------------------------------------------------------
template <typename T>
inline void Animation<T>::addKeyFrame(int index, const KeyFrame& kf)
{
  if (index < 0 || index >= m_properties.size())
    return;

  auto& keyframes = m_properties[index].keyFrames;

  const auto pred = [&](const KeyFrame& f) -> bool
  {
    return f.time > kf.time;
  };
  const auto& it = std::find_if(keyframes.begin(), keyframes.end(), pred);

  if (it == keyframes.end())
  {
    keyframes.push_back(kf);
    return;
  }

  keyframes.insert(it, kf);
}

// ------------------------------------------------------------------------------------------------
template <typename T>
inline void Animation<T>::updateProperties(float time)
{
  const auto pred = [&](KeyFrame f) -> bool
  {
    return f.time > time;
  };

  for (auto& prop : m_properties)
  {
    if (prop.keyFrames.empty())
      continue;

    KeyFrame
      * a = nullptr,
      * b = nullptr;

    const auto& it = std::find_if(prop.keyFrames.begin(), prop.keyFrames.end(), pred);

    // Last Element
    if (it == prop.keyFrames.end())
    {
      a = &(prop.keyFrames.back());
    }

    else
    {
      const auto& index = std::distance(prop.keyFrames.begin(), it);

      // First Element
      if (index == 0)
      {
        b = &(prop.keyFrames[0]);
      }

      // Other Element
      else
      {
        a = &(prop.keyFrames[index - 1]);
        b = &(prop.keyFrames[index]);
      }
    }

    T value = interpolateValue(a, b, time);
    prop.setter(value);
  }
}

// ------------------------------------------------------------------------------------------------
template <typename T>
inline T Animation<T>::interpolateValue(const KeyFrame* a, const KeyFrame* b, float time) const
{
  if (a == nullptr && b == nullptr)
    return T();

  else if (a == nullptr)
    return b->value;

  else if (b == nullptr)
    return a->value;

  float k = (time - a->time) / (b->time - a->time);
  T dist = (b->value - a->value) * k;
  return dist + a->value;
}
