
#pragma once

#include "Component.h"

class AxisCorrector final : public Component
{
public:

  enum Mode
  {
    Y_to_Z = 0,
    Z_to_Y
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