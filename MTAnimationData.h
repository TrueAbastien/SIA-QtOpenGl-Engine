
#pragma once

#include "MTBody.h"

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

public:

  MTAnimationData(const QSharedPointer<MTBody>& body);

  JFVec& data(const QString& key, bool* ok = nullptr);
  JFVec data(const QString& key, bool* ok = nullptr) const;

  JFMap map() const;

private:

  JFMap m_map;
};