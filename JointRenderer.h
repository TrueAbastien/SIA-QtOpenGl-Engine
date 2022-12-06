
#pragma once

#include "Joint.h"
#include "ColoredRenderable.h"


class JointRenderer final : public ColoredRenderable
{
public:

  using Vertices = QVector<VertexData_Colored>;
  using Indices = QVector<GLushort>;

public:

  void init() override;
  void update(UpdateInfo infos) override;

  void addChildren(const Pointer& child) override;

protected:

  void listenChild(Notification notif) override;

private:

  void updateBuffers();
  void updatePositions();

private:
  
  Vertices m_vertices;
  Indices m_indices;
};
