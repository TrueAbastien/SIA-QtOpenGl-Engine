
#pragma once

#include "ColoredRenderable.h"

#include <array>
#include <vector>

class SkinMesh : public ColoredRenderable
{
public:

  using Vertices = std::vector<VertexData_Colored>;
  using Indices = std::vector<GLushort>;

public:

  using Poly = std::vector<GLushort>;
  static void ProcessPoly(Indices& indices, const Poly& poly);

public:

  SkinMesh(const Vertices& vertices, const Indices& indices);

public:

  void init() override;
  void update(UpdateInfo infos) override;

  size_t vSize() const;

private:

  Vertices m_vertices;
  Indices m_indices;
};