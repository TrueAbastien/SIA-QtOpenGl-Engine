
#pragma once

#include "TexturedRenderable.h"
#include "MeshRigRelation.h"

#include <array>
#include <vector>

class MTSkinMesh : public TexturedRenderable
{
public:

  using Vertices = std::vector<VertexData_Textured>;
  using Indices = std::vector<GLushort>;

  using RigPtr = QSharedPointer<MeshRigRelation>;

public:

  using Poly = std::vector<GLushort>;
  static void ProcessPoly(Indices& indices, const Poly& poly);

public:

  MTSkinMesh(const QString& texture, const Vertices& vertices, const Indices& indices);

public:

  void setScale(float scale);
  float scale() const;

  void init() override;
  void update(UpdateInfo infos) override;

  Indices indices() const;
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