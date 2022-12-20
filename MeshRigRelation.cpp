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
    info.worldOrigin = tr * QVector3D(0, 0, 0);
  }
  data.insert(target, info);

  // Compute Child
  for (const auto& child : target->children())
  {
    if (!child.dynamicCast<Joint>().isNull())
    {
      computeJointNode(data, child, tr * child->localToParent());
    }
  }
}

// ------------------------------------------------------------------------------------------------
MeshRigRelation::JointMap MeshRigRelation::computeHomeData(const QSharedPointer<JointRenderer>& body) const
{
  if (body.isNull())
  {
    return MeshRigRelation::JointMap();
  }

  JointMap result;
  computeJointNode(result, body, body->localToWorld());

  return result;
}

// ------------------------------------------------------------------------------------------------
void MeshRigRelation::setWeightData(const VerticesWeight& weights)
{
  m_weights = weights;
}

// ------------------------------------------------------------------------------------------------
void MeshRigRelation::computeWeightData(const JointMap& joints,
                                        const std::vector<VertexData_Colored>& vertices,
                                        const QMatrix4x4& skin_localToWorld)
{
  if (joints.isEmpty())
  {
    return;
  }

  m_weights.clear();
  QMatrix4x4 skin_WorldToLocal = skin_localToWorld.inverted();

  // Copy Joints Map
  JointMap skinLocalJoints;
  for (auto it = joints.cbegin(); it != joints.cend(); ++it)
  {
    JointInfo info = it.value();
    {
      info.worldOrigin = skin_WorldToLocal * info.worldOrigin;
    }
    skinLocalJoints.insert(it.key(), info);
  }

  size_t size = vertices.size();
  m_weights.resize(size);

  // Find Best Weight
  for (size_t ii = 0; ii < size; ++ii)
  {
    auto& weights = m_weights[ii];

    // Find Best Point
    Component::Pointer joint = nullptr;
    float dist = FLT_MAX;

    const auto vtx = vertices[ii];
    for (auto it = skinLocalJoints.cbegin(); it != skinLocalJoints.cend(); ++it)
    {
      QVector3D vtxToJoint = it.value().worldOrigin - vtx.position;

      float dot = QVector3D::dotProduct(vtxToJoint, -vtx.normal);
      if (dot > 0)
      {
        float norm = vtxToJoint.lengthSquared();
        if (norm < dist)
        {
          dist = norm;
          joint = it.key();
        }
      }
    }

    // Add Best Joint
    if (joint != nullptr)
    {
      WeightData data;
      {
        data.weight = 1.0f;
        data.joint = joint;
      }
      weights.push_back(data);
    }
  }
}

// ------------------------------------------------------------------------------------------------
void MeshRigRelation::setupWeight(const JointMap& joints,
                                  const std::vector<VertexData_Colored>& vertices,
                                  const QMatrix4x4& skin_localToWorld)
{
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
      weight.localPosition = joints.value(weight.joint).worldToLocal * vWorldPos;
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
      pos += (tr * data.localPosition) * data.weight;
    }

    vertices[ii].position = worldToLocal * pos;
  }
}
