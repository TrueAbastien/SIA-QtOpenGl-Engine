#include "MeshRigRelation.h"
#include <QRandomGenerator>

// ------------------------------------------------------------------------------------------------
QVector3D randomColor(float seed)
{
    float h = fmod(seed, 360.0);
    float s = 0.8f;
    float v = 0.6f;
    float C = s * v;
    float X = C * (1 - abs(fmod(h / 60.0, 2) - 1));
    float m = v - C;
    QVector3D r;

    if (h < 60)       r = {C, X, 0};
    else if (h < 120) r = {X, C, 0};
    else if (h < 180) r = {0, C, X};
    else if (h < 240) r = {0, X, C};
    else if (h < 300) r = {X, 0, C};
    else              r = {C, 0, X};

    for (int ii = 0; ii < 3; ++ii) r[ii] += m;

    return r;
};

// ------------------------------------------------------------------------------------------------
void computeJointNode(MeshRigRelation::JointMap& data, const Component::Pointer& target, const QMatrix4x4& tr, QRandomGenerator& gen)
{
  if (target.isNull()) return;

  // Target Uniqueness
  if (data.contains(target)) return;

  // Compute Target
  MeshRigRelation::JointInfo info;
  {
    info.worldToLocal = tr.inverted();
    info.worldOrigin = tr * QVector3D(0, 0, 0);
    info.color = randomColor(gen.generateDouble() * 360.0f);
  }
  data.insert(target, info);

  // Compute Child
  for (const auto& child : target->children())
  {
    if (!child.dynamicCast<Joint>().isNull())
    {
      computeJointNode(data, child, tr * child->localToParent(), gen);
    }
  }
}

// ------------------------------------------------------------------------------------------------
MeshRigRelation::JointMap MeshRigRelation::computeHomeData(const Component::Pointer& body) const
{
  if (body.isNull())
  {
    return MeshRigRelation::JointMap();
  }

  JointMap result;
  QRandomGenerator gen;
  computeJointNode(result, body, body->localToWorld(), gen);

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

  // Construct Joints List
  using JointPair = QPair<Component::Pointer, JointInfo>;
  QVector<JointPair> skinLocalJoints;
  for (auto it = joints.cbegin(); it != joints.cend(); ++it)
  {
    const auto parent = it.key();
    const auto children = parent->children();

    // Middle Points
    int childCount = 0;
    for (const auto& child : children)
    {
      if (!joints.contains(child))
      {
        continue;
      }

      const auto data = joints.value(child);
      childCount++;

      JointInfo info = it.value();
      {
        QVector3D middlePoint = info.worldOrigin + 0.5f * (data.worldOrigin - info.worldOrigin);
        info.worldOrigin = skin_WorldToLocal * middlePoint;
      }
      skinLocalJoints.append(JointPair(parent, info));
    }

    // Direct Point
    if (childCount == 0)
    {
      JointInfo info = it.value();
      {
        info.worldOrigin = skin_WorldToLocal * info.worldOrigin;
      }
      skinLocalJoints.append(JointPair(parent, info));
    }
  }

  size_t size = vertices.size();
  m_weights.resize((int) size);

  // Find Best Weight
  for (size_t ii = 0; ii < size; ++ii)
  {
    auto& weights = m_weights[(int) ii];

    // Find Best Point
    Component::Pointer joint = nullptr;
    float factor = FLT_MAX;

    const auto vtx = vertices[ii];
    for (auto it = skinLocalJoints.cbegin(); it != skinLocalJoints.cend(); ++it)
    {
      QVector3D vtxToJoint = it->second.worldOrigin - vtx.position;

      float f = vtxToJoint.lengthSquared();
      if (f < factor)
      {
        factor = f;
        joint = it->first;
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
    auto& weights = m_weights[(int) ii];

    for (auto& weight : weights)
    {
      weight.localPosition = joints.value(weight.joint).worldToLocal * vWorldPos;
    }
  }
}

// ------------------------------------------------------------------------------------------------
void MeshRigRelation::setupColor(const JointMap& joints, std::vector<VertexData_Colored>& vertices)
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

  // Colors
  for (size_t ii = 0; ii < size; ++ii)
  {
    QVector3D color = QVector3D(0, 0, 0);
    for (const auto& weight : m_weights[(int) ii])
    {
      color += weight.weight * joints.value(weight.joint).color;
    }

    vertices[ii].color = color;
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

    for (const auto& data : m_weights[(int) ii])
    {
      QMatrix4x4 tr = data.joint->localToWorld();
      pos += (tr * data.localPosition) * data.weight;
    }

    vertices[ii].position = worldToLocal * pos;
  }
}
