#include "FileWriter.h"

#include <fstream>

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
using WriteBegin = std::function<void(const std::string&, const MTJoint&, bool)>;
using WriteEnd = std::function<void()>;

void writeJoint(const std::string& title, const MTJoint& jt, const WriteBegin& beginFunc, const WriteEnd& endFunc)
{
  beginFunc(title, jt, jt->children.isEmpty());

  for (const auto& child : jt->children)
  {
    std::string childTitle = child->children.isEmpty() ? "END" : "JOINT";
    writeJoint(childTitle, child, beginFunc, endFunc);
  }

  endFunc();
}
bool FileWriter::writeMTBody(const QString& filePath, const MTInput& body)
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
  int depth = 0;

  // Utilities
  const auto append = [&](auto value, bool eol = false)
  {
    file << value << (eol ? "\n" + std::string(depth, '\t') : " ");
  };

  // Write
  const auto writeJointBegin = [&](std::string title, const MTJoint& jt, bool isEnd = false)
  {
    // Title
    append(title);

    // Name
    append(jt->joint->name().toStdString(), true);

    // Bracket
    depth++;
    append('{', true);

    // Offset
    {
      append("OFFSET");

      QVector3D offset = jt->joint->localPosition();
      append(offset[0]);
      append(offset[1]);
      append(offset[2]);
    }

    // Channels
    if (!isEnd)
    {
      append("", true);
      append("CHANNELS 3 Xrotation Yrotation Zrotation");
    }
  };
  const auto writeJointEnd = [&]()
  {
    depth--;
    append("}", true);
  };
  const auto writeHierarchy = [&]()
  {
    append("HIERARCHY", true);

    writeJoint("ROOT", hierarchy->root(), writeJointBegin, writeJointEnd);
  };
  const auto writeMotion = [&]()
  {
    // TODO
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
