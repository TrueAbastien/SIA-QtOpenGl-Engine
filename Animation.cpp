#include "Animation.h"

// ------------------------------------------------------------------------------------------------
void Animation::addProperty(const Property& prop)
{
  m_properties.push_back(prop);
}

// ------------------------------------------------------------------------------------------------
Animation::Property Animation::getProperty(int index) const
{
  if (index < 0 || index >= m_properties.size())
    return Property();

  return m_properties.at(index);
}

// ------------------------------------------------------------------------------------------------
void Animation::clearProperties()
{
  m_properties.clear();
}

// ------------------------------------------------------------------------------------------------
void Animation::addKeyFrame(int index, const KeyFrame& kf)
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
void Animation::updateProperties(float time)
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

    float value = interpolateValue(a, b, time);
    prop.setter(value);
  }
}

// ------------------------------------------------------------------------------------------------
float Animation::interpolateValue(const KeyFrame* a, const KeyFrame* b, float time) const
{
  if (a == nullptr && b == nullptr)
    return 0.0f;

  else if (a == nullptr)
    return b->value;

  else if (b == nullptr)
    return a->value;

  float dist = (b->value - a->value) / (b->time - a->time) * (time - a->time);
  return dist + a->value;
}
