#include "SkinMesh.h"

// ------------------------------------------------------------------------------------------------
void SkinMesh::ProcessPoly(Indices& indices, const Poly& p)
{
  size_t size = p.size();
  if (size < 3) return;

  for (size_t ii = 0; ii < size - 2; ++ii)
  {
    indices.insert(indices.end(), {p[0], p[ii + 2], p[ii + 1]});
  }
}

// ------------------------------------------------------------------------------------------------
SkinMesh::SkinMesh(const Vertices& vertices, const Indices& indices)
  : m_vertices(vertices), m_indices(indices), m_relation(nullptr)
{
}

// ------------------------------------------------------------------------------------------------
void SkinMesh::init()
{
  ColoredRenderable::init();

  Renderable::computeNormals<VertexData_Colored>(m_vertices.data(), m_vertices.size(), m_indices.data(), m_indices.size());

  ColoredRenderable::initRenderable(m_vertices.data(), m_vertices.size(), m_indices.data(), m_indices.size());
}

// ------------------------------------------------------------------------------------------------
void SkinMesh::update(UpdateInfo infos)
{
  ColoredRenderable::update(infos);

  if (!m_relation.isNull())
  {
    m_relation->updatePosition(m_vertices, this->localToWorld());

    Renderable::computeNormals<VertexData_Colored>(m_vertices.data(), m_vertices.size(), m_indices.data(), m_indices.size());

    Renderable::updateVertices<VertexData_Colored>(m_vertices.data(), m_vertices.size());
  }

  ColoredRenderable::updateRenderable(GL_TRIANGLES, m_indices.size());
}

// ------------------------------------------------------------------------------------------------
SkinMesh::Vertices SkinMesh::vertices() const
{
  return m_vertices;
}

// ------------------------------------------------------------------------------------------------
size_t SkinMesh::vSize() const
{
  return m_vertices.size();
}

// ------------------------------------------------------------------------------------------------
void SkinMesh::setRelation(const RigPtr& rig)
{
  m_relation = rig;
}
