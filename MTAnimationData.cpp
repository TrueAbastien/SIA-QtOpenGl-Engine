#include "MTAnimationData.h"

#include "MTAnimatorPlug.h"
#include "MTAnimation.h"

#include <QtMath>

using NodePtr = MTBody::Hierarchy::Node::Pointer;

// ------------------------------------------------------------------------------------------------
struct AscendingData
{
  float acc;
  QVector3D offset;
  QString jtName;
};

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
QVector3D matrixRotation(const QMatrix3x3& m)
{
  float t = QVector2D(m(2, 1), m(2, 2)).length();
  return QVector3D(
    qAtan2(m(2, 1), m(2, 2)),
    qAtan2(-m(2, 0), t),
    qAtan2(m(1, 0), m(0, 0))
  ) * RAD2DEG;
};

// ------------------------------------------------------------------------------------------------
QVector3D positionOf(const QMatrix4x4& model)
{
  return model * QVector3D(0, 0, 0);
}

// ------------------------------------------------------------------------------------------------
bool isDrivingMT(const Component::Pointer& comp, QSharedPointer<MTAnimatorPlug>& animator, QMatrix3x3& invORot)
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
  size_t frameIndex,
  const NodePtr& jointOH, // Original Hierarchy
  const MTBody::BodyMap& bodyMap,
  const MTAnimationData::JointFrame& parentJF,
  MTAnimationData::JFMap& resultMap,
  AscendingData& ascendingData)
{
  QString name = jointOH->joint->name();

  bool hasParent = !jointOH->parent.isNull();
  auto trueJT = bodyMap[name];

  QMatrix3x3 invORot;
  QSharedPointer<MTAnimatorPlug> animator;
  bool isDriving = isDrivingMT(trueJT, animator, invORot);

  QMatrix4x4 parentModel = hasParent ?
    parentJF.localToRoot :
    QMatrix4x4();
  QMatrix4x4 model = isDriving ?
    computeJoint(
      animator->animation()->getProperty(0).keyFrames[frameIndex].value,
      parentModel,
      invORot,
      jointOH->joint->localPosition()) :
    parentModel * jointOH->joint->localToParent();

  static const QStringList __controlNames = { "FOOTL", "FOOTR" };
  if (__controlNames.contains(name) && isDrivingMT)
  {
    // Get Acceleration
    float acc = animator->animation()->getProperty(1).keyFrames[frameIndex].value.length();

    // Saving Offset
    if (acc < ascendingData.acc)
    {
      ascendingData.acc = acc;
      ascendingData.offset = positionOf(model);
      ascendingData.jtName = name;
    }
  }

  QMatrix3x3 localToParentRot =
    parentJF.localToRoot.normalMatrix().transposed()
    * model.normalMatrix();

  MTAnimationData::JointFrame frame;
  {
    frame.localToRoot = model;
    frame.worldOffset = parentJF.worldOffset;
    frame.localToParentRotation = matrixRotation(localToParentRot);
  }
  resultMap[name].append(frame);

  // Compute Children
  for (const auto& child : jointOH->children)
  {
    computeOverChildren(frameIndex, child, bodyMap, frame, resultMap, ascendingData);
  }
}

// ------------------------------------------------------------------------------------------------
size_t frameCount(const NodePtr& root)
{
  QSharedPointer<MTAnimatorPlug> animator;
  for (const auto& child : root->joint->children())
  {
    animator = child.dynamicCast<MTAnimatorPlug>();
    if (!animator.isNull()) break;
  }

  return animator->animation()->getProperty(0).keyFrames.size();
}

// ------------------------------------------------------------------------------------------------
MTAnimationData::MTAnimationData(const QSharedPointer<MTBody>& body)
{
  const auto& bodyMap = body->hierarchyMap();
  const auto& originalHierarchy = body->originalHierarchy();

  size_t count = frameCount(body->hierarchy()->root());

  QVector3D offset;
  AscendingData posData;
  {
    posData.jtName = QString();
  }

  for (size_t ii = 0; ii < count; ++ii)
  {
    posData.acc = FLT_MAX;

    JointFrame frame;
    {
      frame.localToRoot = QMatrix4x4();
      frame.localToParentRotation = QVector3D(0, 0, 0);
      frame.worldOffset = offset;
    }

    AscendingData prevData = posData;

    computeOverChildren(ii, originalHierarchy->root(), bodyMap, frame, m_map, posData);

    if (prevData.jtName == posData.jtName)
    {
      offset += (prevData.offset - posData.offset);
    }
  }
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
