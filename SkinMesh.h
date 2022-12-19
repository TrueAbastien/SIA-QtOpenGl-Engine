
#pragma once

#include "ColoredRenderable.h"
#include "MeshRigRelation.h"

#include <array>
#include <vector>

class SkinMesh : public ColoredRenderable
{
public:

  using Vertices = std::vector<VertexData_Colored>;
  using Indices = std::vector<GLushort>;

  using RigPtr = QSharedPointer<MeshRigRelation>;

public:

  using Poly = std::vector<GLushort>;
  static void ProcessPoly(Indices& indices, const Poly& poly);

public:

  SkinMesh(const Vertices& vertices, const Indices& indices);

public:

  void setScale(float scale);
  float scale() const;

  void init() override;
  void update(UpdateInfo infos) override;

  Vertices vertices() const;
  Vertices& vertices();
  size_t vSize() const;

  void setRelation(const RigPtr& rig);

  void verticesUpdate();

private:

  float m_scale;

  Vertices m_vertices;
  Indices m_indices;

  RigPtr m_relation;
};