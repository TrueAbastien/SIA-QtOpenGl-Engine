
#pragma once

#include "Joint.h"
#include "JointRenderer.h"

#include <QMap>
#include <QVector>

class MeshRigRelation
{
public:

  using JointID = QString;
  struct JointInfo
  {
    QMatrix4x4 localToWorld;
  };
  using JointMap = QMap<JointID, JointInfo>;

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
  void computeHomeData(const QSharedPointer<JointRenderer>& body, const std::vector<VertexData_Colored>& vertices);

  void updatePosition(std::vector<VertexData_Colored>& vertices);


private:

  JointMap m_homeData;
  VerticesWeight m_weights;
};