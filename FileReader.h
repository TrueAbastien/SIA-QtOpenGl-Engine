
#pragma once

#include "JointRenderer.h"

class FileReader final
{
public:

  using BVHResult = QSharedPointer<JointRenderer>;

  struct BVHParameters
  {
    float scale;
  };

public:

  static BVHResult readBVH(const QString& filePath, const BVHParameters& params);
};