
#pragma once

#include "FileReader.h"

class MTBody;

class MTAnimationData final
{
public:

  struct JointFrame
  {
    QMatrix4x4 localToRoot;
    QVector3D localToParentRotation;
    QVector3D worldOffset;
  };

  using JFVec = QVector<JointFrame>;
  using JFMap = QMap<QString, JFVec>;

  using InputData = QMap<QString, FileReader::MTResult>;

public:

  MTAnimationData(MTBody* body, const InputData& data);

  JFVec& data(const QString& key, bool* ok = nullptr);
  JFVec data(const QString& key, bool* ok = nullptr) const;

  JFMap map() const;

  void constructAnimation(MTBody* body, int samplingRate);

private:

  JFMap m_map;
};