
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
  };
  using JointMap = QMap<Component::Pointer, JointInfo>;

  struct WeightData
  {
    Component::Pointer joint;
    float weight;
    QVector3D localPosition;
  };
  using VertexWeight = QVector<WeightData>;
  using VerticesWeight = QVector<VertexWeight>;

public:

  MeshRigRelation() = default;

public:

  void setWeightData(const VerticesWeight& weights);
  void computeHomeData(const QSharedPointer<JointRenderer>& body,
                       const std::vector<VertexData_Colored>& vertices,
                       const QMatrix4x4& skin_localToWorld);

  void updatePosition(std::vector<VertexData_Colored>& vertices,
                      const QMatrix4x4& skin_localToWorld);


private:

  JointMap m_homeData;
  VerticesWeight m_weights;
};