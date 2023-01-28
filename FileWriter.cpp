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
using WriteBegin = std::function<void(const std::string&, const MTJoint&, bool, bool)>;
using WriteEnd = std::function<void()>;

void writeJoint(
  const std::string& title,
  const MTJoint& jt,
  const WriteBegin& beginFunc,
  const WriteEnd& endFunc)
{
  beginFunc(title, jt, jt->children.isEmpty(), jt->parent.isNull());

  for (const auto& child : jt->children)
  {
    std::string childTitle = child->children.isEmpty() ? "END" : "JOINT";
    writeJoint(childTitle, child, beginFunc, endFunc);
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
  std::vector<QVector3D> offsets(0);
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

  // Write
  const auto writeJointBegin = [&](
    std::string title,
    const MTJoint& jt,
    bool isEnd = false,
    bool isRoot = false)
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
      append(isRoot ?
             "CHANNELS 6 Xposition Yposition Zposition Xrotation Yrotation Zrotation" :
             "CHANNELS 3 Xrotation Yrotation Zrotation", true);
    }

    // Animation
    QSharedPointer<MTAnimatorPlug> animator = nullptr;
    const auto children = bodyMap[jt->joint->name()]->children();
    for (const auto& child : children)
    {
      animator = child.dynamicCast<MTAnimatorPlug>();
      if (!animator.isNull()) break;
    }

    if (isEnd) return;

    // Animation
    std::vector<QVector3D> animation(0);
    const auto keyframes = animator->animation()->getProperty(0).keyFrames;
    for (const auto& frame : keyframes)
    {
      // Offset
      if (isRoot)
      {
        offsets.push_back(
          frame.value.localToRoot * QVector3D(0, 0, 0)
          + frame.value.worldOffset);
      }

      // Rotation
      animation.push_back(frame.value.localToParentRotation);
    }

    animations.push_back(animation);
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

    writeJoint("ROOT", hierarchy->root(), writeJointBegin, writeJointEnd);
  };
  const auto writeMotion = [&]()
  {
    append("MOTION", true);

    // Time
    size_t animationCount = offsets.size();
    append("Frames:");
    append(animationCount, true);
    append("Frame Time:");
    append(1.0f / params.sampleRate, true);

    // Values
    for (size_t ii = 0; ii < animationCount; ++ii)
    {
      // Offset
      {
        const auto o = offsets[ii];
        append(o[0]);
        append(o[1]);
        append(o[2]);
      }
      
      // Rotations
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
