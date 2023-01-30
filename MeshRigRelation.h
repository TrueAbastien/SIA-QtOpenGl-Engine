
#pragma once

#include "Joint.h"
#include "JointRenderer.h"

#include <QMap>
#include <QVector>

class MeshRigRelation
{
public:

  struct JointInfo
  {
    QMatrix4x4 worldToLocal;
    QVector3D worldOrigin;
    QVector3D color;
  };
  using JointMap = QMap<BodyBase::Hierarchy::Node::Pointer, JointInfo>;

  struct WeightData
  {
    BodyBase::Hierarchy::Node::Pointer node;
    float weight;
    QVector3D localPosition;
  };
  using VertexWeight = QVector<WeightData>;
  using VerticesWeight = QVector<VertexWeight>;

public:

  MeshRigRelation() = default;

public:

  // Step 1
  JointMap computeHomeData(const QSharedPointer<BodyBase>& body) const;

  // Step 2
  void setWeightData(const VerticesWeight& weights);
  template <typename TVertex>
  void computeWeightData(const JointMap& joints,
                         const std::vector<TVertex>& vertices,
                         const QMatrix4x4& skin_localToWorld);

  // Step 3
  template <typename TVertex>
  void setupWeight(const JointMap& joints,
                   const std::vector<TVertex>& vertices,
                   const QMatrix4x4& skin_localToWorld);

  // Step 3-bis (optional)
  template <typename TVertex>
  void setupColor(const JointMap& joints,
                  std::vector<TVertex>& vertices);

  // Step 4 (to Update)
  template <typename TVertex>
  void updatePosition(std::vector<TVertex>& vertices,
                      const QMatrix4x4& skin_localToWorld);


private:

  VerticesWeight m_weights;
};

// ------------------------------------------------------------------------------------------------
template <typename TVertex>
inline void MeshRigRelation::computeWeightData(
  const JointMap& joints,
  const std::vector<TVertex>& vertices,
  const QMatrix4x4& skin_localToWorld)
{
  if (joints.isEmpty())
  {
    return;
  }

  m_weights.clear();
  QMatrix4x4 skin_WorldToLocal = skin_localToWorld.inverted();

  // Construct Joints List
  using JointPair = QPair<BodyBase::Hierarchy::Node::Pointer, JointInfo>;
  QVector<JointPair> skinLocalJoints;
  for (auto it = joints.cbegin(); it != joints.cend(); ++it)
  {
    const auto parent = it.key();
    const auto children = parent->children;

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
  m_weights.resize((int)size);

  // Find Best Weight
  for (size_t ii = 0; ii < size; ++ii)
  {
    auto& weights = m_weights[(int)ii];

    // Find Best Point
    BodyBase::Hierarchy::Node::Pointer best = nullptr;
    float factor = FLT_MAX;

    const auto vtx = vertices[ii];
    for (auto it = skinLocalJoints.cbegin(); it != skinLocalJoints.cend(); ++it)
    {
      QVector3D vtxToJoint = it->second.worldOrigin - vtx.position;

      float f = vtxToJoint.lengthSquared();
      if (f < factor)
      {
        factor = f;
        best = it->first;
      }
    }

    // Add Best Joint
    if (best != nullptr)
    {
      WeightData data;
      {
        data.weight = 1.0f;
        data.node = best;
      }
      weights.push_back(data);
    }
  }
}

// ------------------------------------------------------------------------------------------------
template <typename TVertex>
inline void MeshRigRelation::setupWeight(
  const JointMap& joints,
  const std::vector<TVertex>& vertices,
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
    auto& weights = m_weights[(int)ii];

    for (auto& weight : weights)
    {
      weight.localPosition = joints.value(weight.node).worldToLocal * vWorldPos;
    }
  }
}

// ------------------------------------------------------------------------------------------------
template <typename TVertex>
inline void MeshRigRelation::setupColor(const JointMap& joints, std::vector<TVertex>& vertices)
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
    for (const auto& weight : m_weights[(int)ii])
    {
      color += weight.weight * joints.value(weight.node).color;
    }

    vertices[ii].color = color;
  }
}

// ------------------------------------------------------------------------------------------------
template <typename TVertex>
inline void MeshRigRelation::updatePosition(
  std::vector<TVertex>& vertices,
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

    for (const auto& data : m_weights[(int)ii])
    {
      QMatrix4x4 tr = data.node->joint->localToWorld();
      pos += (tr * data.localPosition) * data.weight;
    }

    vertices[ii].position = worldToLocal * pos;
  }
}