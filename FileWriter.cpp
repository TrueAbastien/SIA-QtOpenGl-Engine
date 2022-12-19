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
