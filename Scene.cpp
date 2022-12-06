
#include "Scene.h"

// ------------------------------------------------------------------------------------------------
void Scene::init()
{
  initChildren(this);
}

// ------------------------------------------------------------------------------------------------
void Scene::update(UpdateInfo infos)
{
  updateChildren(this, infos);
}

// ------------------------------------------------------------------------------------------------
void Scene::initChildren(Component* component)
{
  // Init Children
  for (const auto& child : component->children())
  {
    child->init();

    initChildren(child.get());
  }
}

// ------------------------------------------------------------------------------------------------
void Scene::updateChildren(Component* component, UpdateInfo _infos)
{
  UpdateInfo infos = _infos;
  infos.screenToParent *= m_localToParent;

  // Update Children
  for (const auto& child : component->children())
  {
    child->update(infos);

    updateChildren(child.get(), infos);
  }
}
