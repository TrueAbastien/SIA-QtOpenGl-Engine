#include "MTAnimationData.h"

#include "MTAnimatorPlug.h"

using NodePtr = MTBody::Hierarchy::Node::Pointer;

// ------------------------------------------------------------------------------------------------
QMatrix3x3 rotationMatrix(const QVector3D& rot)
{
  QMatrix4x4 m = {};
  m.rotate(rot.z(), 0.0, 0.0, 1.0);
  m.rotate(rot.y(), 0.0, 1.0, 0.0);
  m.rotate(rot.x(), 1.0, 0.0, 0.0);

  /*QMatrix3x3 r = {};
  for (int ii = 0; ii < 3; ++ii)
    for (int jj = 0; jj < 3; ++jj)
      r(ii, jj) = m(ii, jj);*/

  return m.normalMatrix();
}

// ------------------------------------------------------------------------------------------------
QVector3D positionOf(const QMatrix4x4& model)
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
QMatrix4x4 computeJoint(
  const QVector3D& worldRotation,
  const QMatrix4x4& model,
  const QMatrix3x3& invORot,
  const QVector3D& localPosition)
{
  QVector3D worldPosition = model * QVector3D(0, 0, 0);

  QMatrix4x4 r(rotationMatrix(worldRotation) * invORot);

  QMatrix4x4 p;
  p.translate(worldPosition);

  QMatrix4x4 t = p * r;
  t.translate(localPosition);

  return t;
}

// ------------------------------------------------------------------------------------------------
void computeOverChildren(
  const NodePtr& jointOH, // Original Hierarchy
  const MTBody::BodyMap& bodyMap,
  const MTAnimationData::JointFrame& parentJF,
  MTAnimationData::JFMap& resultMap)
{
  QString name = jointOH->joint->name();

  bool hasParent = !jointOH->parent.isNull();
  auto trueJT = bodyMap[name];

  QMatrix3x3 invORot;
  bool isDriving = isDrivingMT(trueJT, invORot);

  QMatrix4x4 parentModel = hasParent ?
    parentJF.localToRoot :
    QMatrix4x4();
  QMatrix4x4 model = isDriving ?
    computeJoint(trueJT->localRotation(), parentModel, invORot, jointOH->joint->localPosition()) :
    parentModel * jointOH->joint->localToParent();

  MTAnimationData::JointFrame frame;
  {
    frame.localToRoot = model;
    // TODO: offset & rotation
  }
  resultMap[name].append(frame);

  // Compute Children
  for (const auto& child : jointOH->children)
  {
    computeOverChildren(child, bodyMap, frame, resultMap);
  }
}

// ------------------------------------------------------------------------------------------------
void iterateFindControl(
  const NodePtr& parent,
  NodePtr& result,
  float& minAcc)
{
  static const QStringList __controlNames = { "FOOTL", "FOOTR" };
  const auto isControl = [](const NodePtr& node) -> bool
  {
    return __controlNames.contains(node->joint->name());
  };

  const auto getAcc = [](const NodePtr& node) -> float
  {
    QSharedPointer<MTAnimatorPlug> animator;
    for (const auto& child : node->joint->children())
    {
      animator = child.dynamicCast<MTAnimatorPlug>();
      if (!animator.isNull())
      {
        return animator->acceleration().length();
      }
    }
    return FLT_MAX;
  };

  // Verify Parent
  if (isControl(parent))
  {
    float acc = getAcc(parent);
    if (acc < minAcc)
    {
      minAcc = acc;
      result = parent;
    }
  }

  // Iterate Children
  for (const auto& child : parent->children)
  {
    iterateFindControl(child, result, minAcc);
  }
}

// ------------------------------------------------------------------------------------------------
QVector3D computeGlobalOffset(const NodePtr& root, NodePtr& control)
{
  float minAcc = FLT_MAX;
  iterateFindControl(root, control, minAcc);

  if (control.isNull())
  {
    return QVector3D(0, 0, 0);
  }

  return control->joint->localPosition();
}

// ------------------------------------------------------------------------------------------------
MTAnimationData::MTAnimationData(const QSharedPointer<MTBody>& body)
{
  // TODO
}

// ------------------------------------------------------------------------------------------------
MTAnimationData::JFVec& MTAnimationData::data(const QString& key, bool* ok)
{
  if (ok != nullptr)
  {
    *ok = m_map.contains(key);
  }

  return m_map[key];
}

// ------------------------------------------------------------------------------------------------
MTAnimationData::JFVec MTAnimationData::data(const QString& key, bool* ok) const
{
  if (ok != nullptr)
  {
    *ok = m_map.contains(key);
  }

  return m_map.value(key);
}

// ------------------------------------------------------------------------------------------------
MTAnimationData::JFMap MTAnimationData::map() const
{
  return m_map;
}
