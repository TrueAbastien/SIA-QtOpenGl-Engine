
#pragma once

#include "JointRenderer.h"
#include "SkinMesh.h"

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

public:

  static BVHResult readBVH(const QString& filePath, const BVHParameters& params);

  static OFFResult readOFF(const QString& filePath, const OFFParameters& params);
};