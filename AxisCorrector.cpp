#include "AxisCorrector.h"

// ------------------------------------------------------------------------------------------------
AxisCorrector::AxisCorrector(Mode mode)
  : m_mode(mode)
{
  if (mode == Mode::Y_to_Z || mode == Mode::Z_to_Y)
  {
    m_localToParent = QMatrix4x4(
      1, 0, 0, 0,
      0, 0, 1, 0,
      0, 1, 0, 0,
      0, 0, 0, 1
    );
  }

  //
}

// ------------------------------------------------------------------------------------------------
void AxisCorrector::init()
{
}

// ------------------------------------------------------------------------------------------------
void AxisCorrector::update(UpdateInfo)
{
}

// ------------------------------------------------------------------------------------------------
AxisCorrector::Mode AxisCorrector::mode() const
{
  return m_mode;
}
