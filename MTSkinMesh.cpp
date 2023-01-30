#include "MTSkinMesh.h"

// ------------------------------------------------------------------------------------------------
void MTSkinMesh::ProcessPoly(Indices& indices, const Poly& p)
{
  size_t size = p.size();
  if (size < 3) return;

  for (size_t ii = 0; ii < size - 2; ++ii)
  {
    indices.insert(indices.end(), { p[0], p[ii + 2], p[ii + 1] });
  }
}

// ------------------------------------------------------------------------------------------------
MTSkinMesh::MTSkinMesh(const QString& texture, const Vertices& vertices, const Indices& indices)
  : TexturedRenderable(texture), m_scale(1.0f), m_vertices(vertices), m_indices(indices), m_relation(nullptr)
{
  const auto method = [&](QVector3D pos, QVector3D rot) -> QMatrix4x4
  {
    QMatrix4x4 m = {};
    m.rotate(rot.x(), 1.0, 0.0, 0.0);
    m.rotate(rot.y(), 0.0, 1.0, 0.0);
    m.rotate(rot.z(), 0.0, 0.0, 1.0);
    m.translate(pos);
    m.scale(m_scale);
    return m;
  };
  setMatrixConstruct(method);
}

// ------------------------------------------------------------------------------------------------
void MTSkinMesh::setScale(float scale)
{
  m_scale = scale;

  updateTransform();
}

// ------------------------------------------------------------------------------------------------
float MTSkinMesh::scale() const
{
  return m_scale;
}

// ------------------------------------------------------------------------------------------------
void MTSkinMesh::init()
{
  TexturedRenderable::init();

  Renderable::computeNormals<VertexData_Textured>(
    m_vertices.data(), (int)m_vertices.size(),
    m_indices.data(), (int)m_indices.size(), 3, true);

  TexturedRenderable::initRenderable(
    m_vertices.data(), (int)m_vertices.size(),
    m_indices.data(), (int)m_indices.size());
}

// ------------------------------------------------------------------------------------------------
void MTSkinMesh::update(UpdateInfo infos)
{
  TexturedRenderable::update(infos);

  if (!m_relation.isNull())
  {
    m_relation->updatePosition(m_vertices, this->localToWorld());

    verticesUpdate();
  }

  TexturedRenderable::updateRenderable(GL_TRIANGLES, (int)m_indices.size());
}

// ------------------------------------------------------------------------------------------------
MTSkinMesh::Indices MTSkinMesh::indices() const
{
  return m_indices;
}

// ------------------------------------------------------------------------------------------------
MTSkinMesh::Vertices MTSkinMesh::vertices() const
{
  return m_vertices;
}

// ------------------------------------------------------------------------------------------------
MTSkinMesh::Vertices& MTSkinMesh::vertices()
{
  return m_vertices;
}

// ------------------------------------------------------------------------------------------------
size_t MTSkinMesh::vSize() const
{
  return m_vertices.size();
}

// ------------------------------------------------------------------------------------------------
void MTSkinMesh::setRelation(const RigPtr& rig)
{
  m_relation = rig;
}

// ------------------------------------------------------------------------------------------------
void MTSkinMesh::verticesUpdate()
{
  Renderable::computeNormals<VertexData_Textured>(
    m_vertices.data(), (int)m_vertices.size(),
    m_indices.data(), (int)m_indices.size(), 3, true);

  Renderable::updateVertices<VertexData_Textured>(m_vertices.data(), (int)m_vertices.size());
}
