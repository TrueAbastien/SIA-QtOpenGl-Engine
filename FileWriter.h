
#pragma once

#include "SkinMesh.h"
#include "MTBody.h"

class FileWriter final
{
public:

  using OFFInput = QSharedPointer<SkinMesh>;
  struct OFFParameters
  {
    float scale;
    bool inverted;
  };

  static bool writeOFF(const QString& filePath, const OFFInput& skin, const OFFParameters& params);

  using MTInput = QSharedPointer<MTBody>;

  static bool writeMTBody(const QString& filePath, const MTInput& body);
};
