#include "FileWriter.h"

#include "MTAnimatorPlug.h"
#include "MTAnimation.h"

#include <fstream>
#include <QtMath>

// ------------------------------------------------------------------------------------------------
bool FileWriter::writeOFF(const QString& filePath, const OFFInput& skin, const OFFParameters& params)
{
  if (skin.isNull())
  {
    return false;
  }

  std::ofstream file(filePath.toStdString(), std::fstream::out);
  if (!file.is_open())
  {
    return false;
  }

  // Data
  const auto vertices = skin->vertices();
  const auto indices = skin->indices();

  // Utilities
  const auto append = [&](auto value, bool eol = false)
  {
    file << value << (eol ? "\n" : " ");
  };

  // Writing
  const auto writeHeader = [&]()
  {
    append(vertices.size());
    append(indices.size() / 3);
    append(0, true);
  };
  const auto writeVertices = [&]()
  {
    for (const auto& v : vertices)
    {
      auto p = v.position;
      p *= params.scale;

      append(p.x());
      append(p.y());
      append(p.z(), true);
    }
  };
  const auto writeIndices = [&]()
  {
    for (int ii = 0; ii < indices.size(); ii += 3)
    {
      append(3);
      append(indices[ii + 0]);

      if (params.inverted)
      {
        append(indices[ii + 2]);
        append(indices[ii + 1], true);
      }
      else
      {
        append(indices[ii + 1]);
        append(indices[ii + 2], true);
      }
    }
  };

  bool r = true;
  try
  {
    append("OFF", true);
    writeHeader();
    writeVertices();
    writeIndices();
  }
  catch (...)
  {
    r = false;
  }

  file.close();
  return true;
}

// ------------------------------------------------------------------------------------------------
using MTJoint = MTBody::Hierarchy::Node::Pointer;
using TransformVec = std::vector<QMatrix3x3>;
using WriteBegin = std::function<TransformVec(const std::string&, const MTJoint&, const TransformVec&, bool)>;
using WriteEnd = std::function<void()>;

void writeJoint(
  const std::string& title,
  const MTJoint& jt,
  const TransformVec& trVec,
  const WriteBegin& beginFunc,
  const WriteEnd& endFunc)
{
  auto parentVec = beginFunc(title, jt, trVec, jt->children.isEmpty());

  for (const auto& child : jt->children)
  {
    std::string childTitle = child->children.isEmpty() ? "END" : "JOINT";
    writeJoint(childTitle, child, parentVec, beginFunc, endFunc);
  }

  endFunc();
}
bool FileWriter::writeMTBody(const QString& filePath, const MTInput& body, const MTParameters& params)
{
  if (body.isNull())
  {
    return false;
  }

  std::ofstream file(filePath.toStdString(), std::fstream::out);
  if (!file.is_open())
  {
    return false;
  }

  // Data
  const auto hierarchy = body->originalHierarchy();
  const auto bodyMap = body->hierarchyMap();
  std::vector<std::vector<QVector3D>> animations(0);
  int depth = 0;

  // Utilities
  const auto append = [&](auto value, bool eol = false)
  {
    file << value << (eol ? "\n" : " ");
  };
  const auto tabs = [&]()
  {
    file << std::string(depth, '\t');
  };

  // Convert
  const auto trToRot = [](const QMatrix3x3& r) -> QVector3D
  {
    float t = QVector2D(r(2, 1), r(2, 2)).length();
    return QVector3D(
      qAtan2(r(2, 1), r(2, 2)),
      qAtan2(-r(2, 0), t),
      qAtan2(r(1, 0), r(0, 0))
    ) * RAD2DEG;
  };
  const auto rotToTr = [](const QVector3D& r) -> QMatrix3x3
  {
    QMatrix4x4 m = {};
    m.rotate(r.z(), 0.0, 0.0, 1.0);
    m.rotate(r.y(), 0.0, 1.0, 0.0);
    m.rotate(r.x(), 1.0, 0.0, 0.0);
    return m.normalMatrix();
  };

  // Write
  const auto writeJointBegin = [&](
    std::string title,
    const MTJoint& jt,
    const TransformVec& parentVec,
    bool isEnd = false)
    -> TransformVec
  {
    // Title
    tabs();
    append(title);

    // Name
    append(jt->joint->name().toStdString(), true);

    // Bracket
    tabs();
    append('{', true);
    depth++;

    // Offset
    {
      tabs();
      append("OFFSET");

      QVector3D offset = jt->joint->localPosition();
      append(offset[0]);
      append(offset[1]);
      append(offset[2], true);
    }

    // Channels
    if (!isEnd)
    {
      tabs();
      append("CHANNELS 3 Zrotation Yrotation Xrotation", true);
    }

    // Animation
    QSharedPointer<MTAnimatorPlug> animator = nullptr;
    const auto children = bodyMap[jt->joint->name()]->children();
    for (const auto& child : children)
    {
      animator = child.dynamicCast<MTAnimatorPlug>();
      if (!animator.isNull())
      {
        break;
      }
    }

    if (animator.isNull()) return parentVec;

    // Transform Vec
    TransformVec result(0);
    const auto keyframes = animator->animation()->getProperty(0).keyFrames;
    bool isFirst = parentVec.size() != keyframes.size();
    std::vector<QVector3D> animation(0);
    for (size_t ii = 0; ii < keyframes.size(); ++ii)
    {
      auto localRot = rotToTr(keyframes[ii].value) *
        rotToTr(animator->originalRotation()).transposed();

      if (!isFirst)
      {
        localRot = parentVec[ii].transposed() * localRot;
      }

      result.push_back(localRot);
      animation.push_back(trToRot(localRot));
    }

    animations.push_back(animation);
    return result;
  };
  const auto writeJointEnd = [&]()
  {
    depth--;
    tabs();
    append("}", true);
  };
  const auto writeHierarchy = [&]()
  {
    append("HIERARCHY", true);

    writeJoint("ROOT", hierarchy->root(), TransformVec(0), writeJointBegin, writeJointEnd);
  };
  const auto writeMotion = [&]()
  {
    append("MOTION", true);

    // Time
    size_t animationCount = animations.front().size();
    append("Frames:");
    append(animationCount, true);
    append("Frame Time:");
    append(1.0f / params.sampleRate, true);

    // Values
    for (size_t ii = 0; ii < animationCount; ++ii)
    {
      for (size_t jj = 0; jj < animations.size(); ++jj)
      {
        bool isEnd = (jj == animations.size() - 1);
        
        const auto v = animations[jj][ii];
        append(v[0]);
        append(v[1]);
        append(v[2], isEnd);
      }
    }
  };

  // Algorithm
  bool r = true;
  try
  {
    writeHierarchy();
    writeMotion();
  }
  catch (...)
  {
    r = false;
  }

  file.close();
  return true;
}
