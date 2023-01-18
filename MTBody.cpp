#include "MTBody.h"

#include "MTAnimatorPlug.h"

// ------------------------------------------------------------------------------------------------
QMatrix3x3 rotationMatrix(const QVector3D& rot)
{
  QMatrix4x4 m = {};
  m.rotate(rot.z(), 0.0, 0.0, 1.0);
  m.rotate(rot.y(), 0.0, 1.0, 0.0);
  m.rotate(rot.x(), 1.0, 0.0, 0.0);

  QMatrix3x3 r = {};
  for (int ii = 0; ii < 3; ++ii)
    for (int jj = 0; jj < 3; ++jj)
      r(ii, jj) = m(ii, jj);

  return r;
}

// ------------------------------------------------------------------------------------------------
QVector3D positionVector(const QMatrix4x4& model)
{
  return model * QVector3D(0, 0, 0);
}

// ------------------------------------------------------------------------------------------------
bool isDrivingMT(const Component::Pointer& comp, QMatrix3x3& invORot)
{
  for (const auto& child : comp->children())
  {
    auto item = child.dynamicCast<MTAnimatorPlug>();
    if (!item.isNull())
    {
      invORot = rotationMatrix(item->originalRotation()).transposed();
      return true;
    }
  }

  return false;
}

// ------------------------------------------------------------------------------------------------
QMatrix4x4 compositeModel(const QVector3D& worldRotation, const QMatrix4x4& model, const QMatrix3x3& invORot, const QVector3D& localPosition)
{
  QVector3D worldPosition = model * QVector3D(0, 0, 0);

  QMatrix4x4 p;
  p.translate(worldPosition);

  QMatrix4x4 r(rotationMatrix(worldRotation) * invORot);

  QMatrix4x4 t = p * r;
  t.translate(localPosition);

  return t;
}

// ------------------------------------------------------------------------------------------------
void constructOverChildren(const Component* parent, const QMatrix4x4& model,
  MTBody::Vertices& vts, MTBody::Indices& ids, MTBody::BodyMap& map, MTBody* body)
{
  VertexData_Wired parent_vtx;
  {
    parent_vtx.position = positionVector(model);
    parent_vtx.color = QVector3D(0, 0, 0.8f);
  }
  GLushort parent_idx = vts.size();

  vts.push_back(parent_vtx);

  // Joint Item
  {
    auto jt = QSharedPointer<Joint>::create();
    jt->setName(parent->name());
    jt->setLocalPosition(positionVector(model));
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
      child_vtx.position = positionVector(newModel);
      child_vtx.color = QVector3D(0.8f, 0.4f, 0);
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
  auto jt = map[parent->name()];

  QMatrix3x3 invORot;
  bool isDriving = isDrivingMT(jt, invORot);

  vts[++jointIndex].position = positionVector(model);
  jt->setLocalPosition(positionVector(model));

  // Update Children
  for (const auto& child : parent->children())
  {
    // Ensure Child is joint
    if (child.isNull() || child.dynamicCast<Joint>().isNull()) continue;

    QMatrix4x4 newModel = isDriving ?
      compositeModel(jt->localRotation(), model, invORot, child->localPosition()) :
      model * child->localToParent();
    vts[++jointIndex].position = positionVector(newModel);

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
