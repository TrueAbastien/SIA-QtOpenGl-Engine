
#pragma once

#include "JointRenderer.h"
#include "SkinMesh.h"
#include "MeshRigRelation.h"
#include "MTSkinMesh.h"

#include <QMap>

class FileReader final
{
public:

  using BVHResult = QSharedPointer<JointRenderer>;
  struct BVHParameters
  {
    float scale;
    bool yzInverted;
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

  struct MTResultFrame
  {
    QVector3D acceleration;
    QVector3D rotation;
  };
  using MTResult = QVector<MTResultFrame>;

  using MTMappingResult = QSharedPointer<QMap<QString, QString>>;

  using MTSkinResult = QSharedPointer<MTSkinMesh>;
  struct MTSkinParameters
  {
    QString texture;
    float scale;
  };

public:

  static BVHResult readBVH(const QString& filePath, const BVHParameters& params);

  static OFFResult readOFF(const QString& filePath, const OFFParameters& params);

  static WeightResult readWeight(const QString& filePath, const WeightParameters& params);

  static MTResult readMT(const QString& filePath);

  static MTMappingResult readMTMapping(const QString& filePath);

  static MTSkinResult readMTSkin(const QString& filePath, const MTSkinParameters& params);
};