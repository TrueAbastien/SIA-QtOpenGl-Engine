
#pragma once

#include "JointRenderer.h"
#include "SkinMesh.h"
#include "MeshRigRelation.h"
#include "MTAnimatorPlug.h"

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
    BVHResult root;
    OFFResult skin;
  };

  using MTResult = QSharedPointer<MTAnimatorPlug>;
  struct MTParameters
  {
    Component::Pointer parent;
    int samplingRate;
  };

public:

  static BVHResult readBVH(const QString& filePath, const BVHParameters& params);

  static OFFResult readOFF(const QString& filePath, const OFFParameters& params);

  static WeightResult readWeight(const QString& filePath, const WeightParameters& params);

  static MTResult readMT(const QString& filePath, const MTParameters& params);
};