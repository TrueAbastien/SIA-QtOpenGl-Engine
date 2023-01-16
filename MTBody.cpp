#include "MTBody.h"

// ------------------------------------------------------------------------------------------------
QMatrix4x4 compositeModel(const QVector3D& worldRotation, const QMatrix4x4& model, const QVector3D& localPosition)
{
  QVector3D worldPosition = model * localPosition;

  const auto method = [](QVector3D pos, QVector3D rot) -> QMatrix4x4
  {
    QMatrix4x4 m = {};
    m.rotate(rot.x(), 1.0, 0.0, 0.0);
    m.rotate(rot.y(), 0.0, 1.0, 0.0);
    m.rotate(rot.z(), 0.0, 0.0, 1.0);
    m.translate(pos);
    return m;
  };

  return method(worldPosition, worldRotation);
}

// ------------------------------------------------------------------------------------------------
void constructOverChildren(const Component* parent, const QMatrix4x4& model,
  MTBody::Vertices& vts, MTBody::Indices& ids, MTBody::BodyMap& map, MTBody* body)
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
    auto jt = QSharedPointer<Joint>::create();
    jt->setName(parent->name());
    jt->setLocalPosition(model * QVector3D(0, 0, 0));
    body->addChildren(jt);

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
      child_vtx.color = QVector3D(0.8f, 0.8f, 0);
    }

    ids.push_back(parent_idx);
    ids.push_back((GLushort)vts.size());
    vts.push_back(child_vtx);

    constructOverChildren(child.get(), newModel, vts, ids, map, body);
  }
}

// ------------------------------------------------------------------------------------------------
void updateOverChildren(const Component* parent, const QMatrix4x4& model,
  JointRenderer::Vertices& vts, const MTBody::BodyMap& map, int& jointIndex)
{
  vts[++jointIndex].position = model * QVector3D(0, 0, 0);

  auto jt = map[parent->name()];
  jt->setLocalPosition(model * QVector3D(0, 0, 0));

  // Update Children
  for (const auto& child : parent->children())
  {
    // Ensure Child is joint
    if (child.isNull() || child.dynamicCast<Joint>().isNull()) continue;

    QMatrix4x4 newModel = compositeModel(jt->localRotation(), model, child->localPosition());
    vts[++jointIndex].position = newModel * QVector3D(0, 0, 0);

    updateOverChildren(child.get(), newModel, vts, map, jointIndex);
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

  constructOverChildren(m_body.get(), QMatrix4x4(), m_vertices, m_indices, m_bodyMap, this);

  WiredRenderable::initRenderable(
    m_vertices.data(), m_vertices.size(),
    m_indices.data(), m_indices.size());
}

// ------------------------------------------------------------------------------------------------
void MTBody::update(UpdateInfo infos)
{
  WiredRenderable::update(infos);

  updatePositions();

  WiredRenderable::updateRenderable(GL_LINES, m_indices.size());
}

// ------------------------------------------------------------------------------------------------
void MTBody::updatePositions()
{
  int jtIndex = -1;

  updateOverChildren(m_body.get(), QMatrix4x4(), m_vertices, m_bodyMap, jtIndex);

  // Update Buffers Infos
  Renderable::updateVertices<VertexData_Wired>(m_vertices.data(), m_vertices.size());
}
