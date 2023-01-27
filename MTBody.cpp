#include "MTBody.h"

#include "MTAnimatorPlug.h"

// ------------------------------------------------------------------------------------------------
void constructOverChildren(
  const Component* parent,
  const QMatrix4x4& model,
  MTBody::Vertices& vts,
  MTBody::Indices& ids,
  MTBody::BodyMap& map,
  MTBody* body)
{
  VertexData_Wired parent_vtx;
  {
    parent_vtx.position = model * QVector3D(0, 0, 0);
    parent_vtx.color = QVector3D(0, 0, 0.8f);
  }
  GLushort parent_idx = vts.size();

  vts.push_back(parent_vtx);

  // Joint Item
  {
    const auto method = [=](QVector3D, QVector3D)
    {
      return model;
    };

    auto jt = QSharedPointer<Joint>::create();
    jt->setName(parent->name());
    jt->setMatrixConstruct(method);
    body->addChildren(jt);

    auto animator = QSharedPointer<MTAnimatorPlug>::create();
    jt->addChildren(animator);

    // Map Binding
    map.insert(jt->name(), jt);
  }

  // Add Children
  for (const auto& child : parent->children())
  {
    // Ensure Child is joint
    if (child.isNull() || child.dynamicCast<Joint>().isNull()) continue;

    QMatrix4x4 newModel = model * child->localToParent();

    VertexData_Wired child_vtx;
    {
      child_vtx.position = newModel * QVector3D(0, 0, 0);
      child_vtx.color = QVector3D(0.8f, 0.4f, 0);
    }

    ids.push_back(parent_idx);
    ids.push_back((GLushort)vts.size());
    vts.push_back(child_vtx);

    constructOverChildren(child.get(), newModel, vts, ids, map, body);
  }
}

// ------------------------------------------------------------------------------------------------
QVector3D positionVector(const MTBody::Hierarchy::Node::Pointer& node)
{
  return node->joint->localToParent() * QVector3D(0, 0, 0);
}

// ------------------------------------------------------------------------------------------------
void updateOverChildren(
  const MTBody::Hierarchy::Node::Pointer& node,
  JointRenderer::Vertices& vts,
  int& jointIndex)
{
  vts[++jointIndex].position = positionVector(node);

  // Update Children
  for (const auto& child : node->children)
  {
    vts[++jointIndex].position = positionVector(child);

    updateOverChildren(child, vts, jointIndex);
  }
}

// ------------------------------------------------------------------------------------------------
MTBody::MTBody(const Body& body)
  : m_body(body), m_vertices(0), m_indices(0)
{
}

// ------------------------------------------------------------------------------------------------
void MTBody::init()
{
  WiredRenderable::init();

  constructOverChildren(m_body.get(), m_body->localToParent(), m_vertices, m_indices, m_bodyMap, this);

  WiredRenderable::initRenderable(
    m_vertices.data(), m_vertices.size(),
    m_indices.data(), m_indices.size());

  resetHierarchy();
}

// ------------------------------------------------------------------------------------------------
void MTBody::update(UpdateInfo infos)
{
  WiredRenderable::update(infos);

  updatePositions();

  WiredRenderable::updateRenderable(GL_LINES, m_indices.size());
}

// ------------------------------------------------------------------------------------------------
MTBody::Hierarchy::Pointer MTBody::computeHierarchy() const
{
  if (m_body.isNull())
  {
    return nullptr;
  }

  auto result = m_body->computeHierarchy();

  // Convert for MT Body
  Hierarchy::Node::Vector next = { result->root() };
  while (!next.isEmpty())
  {
    auto curr = std::move(next);
    next.clear();

    for (const auto& e : curr)
    {
      e->joint = m_bodyMap[e->joint->name()].get();
      next.append(e->children);
    }
  }

  return result;
}

// ------------------------------------------------------------------------------------------------
MTBody::Hierarchy::Pointer MTBody::originalHierarchy()
{
  return m_body->hierarchy();
}

// ------------------------------------------------------------------------------------------------
MTBody::BodyMap MTBody::hierarchyMap() const
{
  return m_bodyMap;
}

// ------------------------------------------------------------------------------------------------
void MTBody::setup(const MTAnimationData::InputData& input, int samplingRate)
{
  if (!m_animationData.isNull())
  {
    return;
  }

  m_animationData = AnimationDataPtr::create(this, input);
  m_animationData->constructAnimation(this, samplingRate);
}

// ------------------------------------------------------------------------------------------------
void MTBody::updatePositions()
{
  int jtIndex = -1;

  updateOverChildren(hierarchy()->root(), m_vertices, jtIndex);

  // Update Buffers Infos
  Renderable::updateVertices<VertexData_Wired>(m_vertices.data(), m_vertices.size());
}
