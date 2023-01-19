
#pragma once

#include "JointRenderer.h"
#include "SkinMesh.h"
#include "MeshRigRelation.h"
#include "MTAnimatorPlug.h"

#include <QMap>

class FileReader final
{
public:

  using BVHResult = QSharedPointer<JointRenderer>;
  struct BVHParameters
  {
    float scale;
  };

  using OFFResult = QSharedPointer<SkinMesh>;
  struct OFFParameters
  {
    float scale;
  };

  using WeightResult = QSharedPointer<MeshRigRelation::VerticesWeight>;
  struct WeightParameters
  {
    QSharedPointer<BodyBase> body;
    OFFResult skin;
  };

  using MTResult = QSharedPointer<MTAnimatorPlug>;
  struct MTParameters
  {
    Component::Pointer parent;
    int samplingRate;
  };

  using MTMappingResult = QSharedPointer<QMap<QString, QString>>;

public:

  static BVHResult readBVH(const QString& filePath, const BVHParameters& params);

  static OFFResult readOFF(const QString& filePath, const OFFParameters& params);

  static WeightResult readWeight(const QString& filePath, const WeightParameters& params);

  static MTResult readMT(const QString& filePath, const MTParameters& params);

  static MTMappingResult readMTMapping(const QString& filePath);
};