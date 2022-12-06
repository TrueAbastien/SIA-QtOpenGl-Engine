
#include "Joint.h"

// ------------------------------------------------------------------------------------------------
void Joint::init()
{
}

// ------------------------------------------------------------------------------------------------
void Joint::update(UpdateInfo infos)
{
}

// ------------------------------------------------------------------------------------------------
void Joint::addChildren(const Pointer& child)
{
  Component::addChildren(child);

  if (child.isNull() || child.dynamicCast<Joint>().isNull()) return;

  Component::notifyParent(Notification::CHILD_ADDED);
}
