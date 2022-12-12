
#pragma once

#include "JointRenderer.h"

class FileReader final
{
public:

  using BVHResult = QSharedPointer<JointRenderer>;

public:

  static BVHResult readBVH(const QString& filePath);
};