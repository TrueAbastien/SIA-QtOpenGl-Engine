
#pragma once

#include "Component.h"

class AxisCorrector final : public Component
{
public:

  enum Mode
  {
    None = 0,
    Y_Z
  };

public:

  AxisCorrector(Mode mode);

public:

  void init() override;
  void update(UpdateInfo info) override;

  Mode mode() const;

private:

  Mode m_mode;
};