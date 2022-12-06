#include "JointRenderer.h"

// ------------------------------------------------------------------------------------------------
void constructOverChildren(const Component* root, const QMatrix4x4& model,
                           JointRenderer::Vertices& vts, JointRenderer::Indices& ids)
{
  VertexData_Colored parent_vtx
  {
    model * QVector3D(0, 0, 0),
    QVector3D(0, 0.8, 0)
  };
  GLushort parent_idx = vts.size();

  vts.push_back(parent_vtx);

  // Add Children
  for (const auto& child : root->children())
  {
    // Ensure Child is joint
    if (child.isNull() || child.dynamicCast<Joint>().isNull()) continue;

    QMatrix4x4 newModel = model * child->localToParent();

    VertexData_Colored child_vtx
    {
      newModel * QVector3D(0, 0, 0),
      QVector3D(0.8, 0, 0)
    };

    ids.push_back(parent_idx);
    ids.push_back((GLushort) vts.size());
    vts.push_back(child_vtx);

    constructOverChildren(child.get(), newModel, vts, ids);
  }
}

// ------------------------------------------------------------------------------------------------
void updateOverChildren(const Component* root, const QMatrix4x4& model,
                        JointRenderer::Vertices& vts, int& jointIndex)
{
  vts[++jointIndex].position = model * QVector3D(0, 0, 0);

  // Update Children
  for (const auto& child : root->children())
  {
    // Ensure Child is joint
    if (child.isNull() || child.dynamicCast<Joint>().isNull()) continue;

    QMatrix4x4 newModel = model * child->localToParent();
    vts[++jointIndex].position = newModel * QVector3D(0, 0, 0);

    updateOverChildren(child.get(), newModel, vts, jointIndex);
  }
}

// ------------------------------------------------------------------------------------------------
void JointRenderer::init()
{
  ColoredRenderable::init();

  updateBuffers();
}

// ------------------------------------------------------------------------------------------------
void JointRenderer::update(UpdateInfo infos)
{
  ColoredRenderable::update(infos);

  updatePositions();

  ColoredRenderable::updateRenderable(GL_LINES, m_indices.size());
}

// ------------------------------------------------------------------------------------------------
void JointRenderer::addChildren(const Pointer& child)
{
  Component::addChildren(child);

  updateBuffers();
}

// ------------------------------------------------------------------------------------------------
void JointRenderer::listenChild(Notification notif)
{
  if (notif == Notification::CHILD_ADDED)
  {
    updateBuffers();
  }
}

// ------------------------------------------------------------------------------------------------
void JointRenderer::updateBuffers()
{
  m_vertices.clear();
  m_indices.clear();

  constructOverChildren(this, QMatrix4x4(), m_vertices, m_indices);

  // Recompute Buffers Infos
  ColoredRenderable::initRenderable(m_vertices.data(), m_vertices.size(),
                                    m_indices.data(), m_indices.size());
}

// ------------------------------------------------------------------------------------------------
void JointRenderer::updatePositions()
{
  int jtIndex = -1;

  updateOverChildren(this, QMatrix4x4(), m_vertices, jtIndex);

  // Update Buffers Infos
  Renderable::updateVertices<VertexData_Colored>(m_vertices.data(), m_vertices.size());
}
