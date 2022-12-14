
#pragma once

#include "Joint.h"
#include "WiredRenderable.h"


class JointRenderer final : public WiredRenderable
{
public:

  using Vertices = QVector<VertexData_Wired>;
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
