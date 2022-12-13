#include "MeshRigRelation.h"

// ------------------------------------------------------------------------------------------------
void computeJointNode(MeshRigRelation::JointMap& data, const Component::Pointer& target, const QMatrix4x4& tr)
{
  if (target.isNull()) return;

  // Target Uniqueness
  if (data.contains(target)) return;

  // Compute Target
  MeshRigRelation::JointInfo info;
  {
    info.worldToLocal = tr.inverted();
  }
  data.insert(target, info);

  // Compute Child
  for (const auto& child : target->children())
  {
    if (!target.dynamicCast<Joint>().isNull())
    {
      computeJointNode(data, child, tr * child->localToParent());
    }
  }
}

// ------------------------------------------------------------------------------------------------
void MeshRigRelation::setWeightData(const VerticesWeight& weights)
{
  m_weights = weights;
}

// ------------------------------------------------------------------------------------------------
void MeshRigRelation::computeHomeData(const QSharedPointer<JointRenderer>& body,
                                      const std::vector<VertexData_Colored>& vertices,
                                      const QMatrix4x4& skin_localToWorld)
{
  if (body.isNull())
  {
    return;
  }

  // Home Data
  m_homeData.clear();
  computeJointNode(m_homeData, body, body->localToWorld());

  if (m_weights.isEmpty())
  {
    return;
  }

  size_t size = vertices.size();
  if (size != m_weights.size())
  {
    return;
  }

  // Local Positions
  for (size_t ii = 0; ii < size; ++ii)
  {
    QVector3D vWorldPos = skin_localToWorld * vertices[ii].position;
    auto& weights = m_weights[ii];

    for (auto& weight : weights)
    {
      weight.localPosition = m_homeData[weight.joint].worldToLocal * vWorldPos;
    }
  }
}

// ------------------------------------------------------------------------------------------------
void MeshRigRelation::updatePosition(std::vector<VertexData_Colored>& vertices,
                                     const QMatrix4x4& skin_localToWorld)
{
  size_t size = vertices.size();
  if (size != m_weights.size())
  {
    return;
  }

  QMatrix4x4 worldToLocal = skin_localToWorld.inverted();

  for (size_t ii = 0; ii < vertices.size(); ++ii)
  {
    QVector3D pos;

    for (const auto& data : m_weights[ii])
    {
      QMatrix4x4 tr = data.joint->localToWorld();
      pos += data.weight * tr * data.localPosition;
    }

    vertices[ii].position = worldToLocal * pos;
  }
}
