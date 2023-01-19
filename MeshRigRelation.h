
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
  void computeWeightData(const JointMap& joints,
                         const std::vector<VertexData_Colored>& vertices,
                         const QMatrix4x4& skin_localToWorld);

  // Step 3
  void setupWeight(const JointMap& joints,
                   const std::vector<VertexData_Colored>& vertices,
                   const QMatrix4x4& skin_localToWorld);

  // Step 3-bis (optional)
  void setupColor(const JointMap& joints,
                  std::vector<VertexData_Colored>& vertices);

  // Step 4 (to Update)
  void updatePosition(std::vector<VertexData_Colored>& vertices,
                      const QMatrix4x4& skin_localToWorld);


private:

  VerticesWeight m_weights;
};