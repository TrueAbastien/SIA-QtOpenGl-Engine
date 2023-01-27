
#pragma once

#include "JointRenderer.h"
#include "MTAnimationData.h"

class MTBody final : public BodyBase
{
public:

  using Body = QSharedPointer<JointRenderer>;
  using BodyMap = QMap<QString, Component::Pointer>;

  using Vertices = QVector<VertexData_Wired>;
  using Indices = QVector<GLushort>;

  using AnimationDataPtr = QSharedPointer<MTAnimationData>;

public:

  MTBody(const Body& body);

public:

  void init() override;
  void update(UpdateInfo infos) override;
  
  Hierarchy::Pointer computeHierarchy() const override;
  Hierarchy::Pointer originalHierarchy();

  BodyMap hierarchyMap() const;

  void setup(const MTAnimationData::InputData& input, int samplingRate = 60);

private:

  void updatePositions();

private:

  Body m_body;
  BodyMap m_bodyMap;

  AnimationDataPtr m_animationData;

  Vertices m_vertices;
  Indices m_indices;

  QVector3D m_offset;
};