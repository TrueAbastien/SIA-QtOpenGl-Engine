
#pragma once

#include "Component.h"

class Scene : public Component
{
public:

  Scene() = default;

public:

  void init() override;
  void update(UpdateInfo infos) override;

  void addChildren(const Pointer& child) override;

private:
  
  void initChildren(Component* component);
  void updateChildren(Component* component, UpdateInfo infos);
};