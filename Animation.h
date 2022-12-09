
#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <functional>


class Animation
{
public:

  using PropertyCallback = std::function<void(float)>;

  struct KeyFrame
  {
    float time;
    float value;
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
  void clearProperties();

  void addKeyFrame(int index, const KeyFrame& kf);

  void updateProperties(float time);

protected:

  virtual float interpolateValue(const KeyFrame* a, const KeyFrame* b, float time) const;

protected:

  std::vector<Property> m_properties;
};