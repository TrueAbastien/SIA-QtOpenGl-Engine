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
void computeJointNode(
  MeshRigRelation::JointMap& data,
  const BodyBase::Hierarchy::Node::Pointer& target,
  QRandomGenerator& gen)
{
  if (target.isNull()) return;

  // Target Uniqueness
  if (data.contains(target)) return;

  auto tr = target->joint->localToWorld();

  // Compute Target
  MeshRigRelation::JointInfo info;
  {
    info.worldToLocal = tr.inverted();
    info.worldOrigin = tr * QVector3D(0, 0, 0);
    info.color = randomColor(gen.generateDouble() * 360.0f);
  }
  data.insert(target, info);

  // Compute Child
  for (const auto& child : target->children)
  {
    computeJointNode(data, child, gen);
  }
}

// ------------------------------------------------------------------------------------------------
MeshRigRelation::JointMap MeshRigRelation::computeHomeData(const QSharedPointer<BodyBase>& body) const
{
  if (body.isNull())
  {
    return MeshRigRelation::JointMap();
  }

  auto hierarchy = body->hierarchy();

  JointMap result;
  QRandomGenerator gen;
  computeJointNode(result, hierarchy->root(), gen);

  return result;
}

// ------------------------------------------------------------------------------------------------
void MeshRigRelation::setWeightData(const VerticesWeight& weights)
{
  m_weights = weights;
}
