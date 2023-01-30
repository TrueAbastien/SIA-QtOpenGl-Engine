#include "FileReader.h"

#include "AnimatorPlug.h"
#include "MTBody.h"

#include <QRandomGenerator>
#include <QFileInfo>
#include <QDir>
#include <QtMath>

#include <fstream>
#include <functional>
#include <algorithm>
#include <numeric>
#include <sstream>

// ------------------------------------------------------------------------------------------------
enum DofType
{
  Position = 0,
  Rotation,

  __COUNT__
};
struct AnimationData
{
  std::vector<QVector3D> keyframes;
  DofType type;
  int ordering[3] = {0,1,2};
};
struct JointData
{
  QString name;
  QVector3D offset;

  std::vector<AnimationData> dofs;
  JointData* parent;
};
using JointDataVec = std::vector<QSharedPointer<JointData>>;

// ------------------------------------------------------------------------------------------------
FileReader::BVHResult FileReader::readBVH(const QString& filePath, const BVHParameters& params)
{
  auto file = std::ifstream(filePath.toStdString());
  if (!file.is_open())
  {
    return nullptr;
  }

  // Data
  auto result = BVHResult::create();
  JointDataVec joints(0);
  JointData* curr = nullptr;
  std::string token;
  float dt;
  int nframes;

  // Utilities
  const auto lassert = [](bool cond)
  {
#ifdef _DEBUG
    assert(cond);
#else
    if (!cond) throw false;
#endif
  };
  const auto next = [&](auto& value)
  {
    file >> std::skipws >> value;
  };
  const auto skip = [&]()-> std::string
  {
    std::string val; next(val);
    return val;
  };
  const auto upper = [](std::string s) -> std::string
  {
    std::string r;

    std::transform(s.begin(), s.end(), std::back_inserter(r), [](char c)
                   {
                     return std::toupper(c);
                   });

    return r;
  };

  // Hierarchy
  const auto readJoint = [&](bool isEnd) -> QSharedPointer<JointData>
  {
    auto joint = QSharedPointer<JointData>::create();

    // Increment
    joint->parent = curr;
    curr = joint.get();

    // Name
    {
      std::string name; next(name);
      joint->name = QString::fromStdString(name);
    }

    lassert(skip() == "{");

    // Offset
    {
      lassert(upper(skip()) == "OFFSET");

      for (int ii = 0; ii < 3; ++ii)
      {
        double val; next(val);
        joint->offset[ii] = (float) val;
      }
    }

    // Channels
    if (!isEnd)
    {
      lassert(upper(skip()) == "CHANNELS");

      int n; next(n); n /= 3;
      joint->dofs = std::vector<AnimationData>(n);

      for (int ii = 0; ii < n; ++ii)
      {
        AnimationData& anim = joint->dofs[ii];

        for (int jj = 0; jj < 3; ++jj)
        {
          std::string symbol; next(symbol);
          symbol = upper(symbol);

          // Position
          if (symbol == "XPOSITION")
          {
            anim.ordering[jj] = 0;
            anim.type = DofType::Position;
          }
          else if (symbol == "YPOSITION")
          {
            anim.ordering[jj] = 1;
            anim.type = DofType::Position;
          }
          else if (symbol == "ZPOSITION")
          {
            anim.ordering[jj] = 2;
            anim.type = DofType::Position;
          }

          // Rotation
          else if (symbol == "XROTATION")
          {
            anim.ordering[jj] = 0;
            anim.type = DofType::Rotation;
          }
          else if (symbol == "YROTATION")
          {
            anim.ordering[jj] = 1;
            anim.type = DofType::Rotation;
          }
          else if (symbol == "ZROTATION")
          {
            anim.ordering[jj] = 2;
            anim.type = DofType::Rotation;
          }

          // Error
          else lassert(false);
        }
      }
    }

    return joint;
  };
  const auto readHierarchy = [&]()
  {
    lassert(upper(skip()) == "ROOT");

    do
    {
      // Descent
      do
      {
        joints.push_back(readJoint(false));

        token = upper(skip());
      }
      while (token == "JOINT");

      // End Joint
      lassert(token == "END");
      joints.push_back(readJoint(true));

      // Ascent
      while ((token = skip()) == "}")
      {
        curr = curr->parent;
      }
    }
    while (curr != nullptr);
  };

  // Motion
  const auto readMotion = [&]()
  {
    // Frame Count
    lassert(skip() == "Frames:");
    next(nframes);
    
    // Frame Time
    lassert(skip() == "Frame");
    lassert(skip() == "Time:");
    next(dt);

    // Key Frames Value
    for (int ii = 0; ii < nframes; ++ii)
    {
      for (auto& jt : joints)
      {
        auto& anims = jt->dofs;

        for (size_t jj = 0; jj < anims.size(); ++jj)
        {
          auto& anim = anims[jj];

          QVector3D res;
          for (int kk = 0; kk < 3; ++kk)
          {
            float val; next(val);
            res[anim.ordering[kk]] = val;
          }
          anim.keyframes.push_back(res);
        }
      }
    }
  };

  // Reader
  try
  {
    lassert(upper(skip()) == "HIERARCHY");
    readHierarchy();

    if (upper(token) == "MOTION")
      readMotion();
  }
  catch (...)
  {
    return nullptr;
  }

  // Adapter
  {
    std::vector<Component::Pointer> jointComponents(0);

    // Translate
    for (const auto& jt : joints)
    {
      // Joint Base
      auto joint = QSharedPointer<Joint>::create();
      joint->setName(jt->name);
      joint->setLocalPosition(jt->offset * params.scale);

      // Animation
      if (!jt->dofs.empty())
      {
        auto anim = QSharedPointer<AnimatorPlug>::create();
        for (const auto& dof : jt->dofs)
        {
          float mult = (dof.type == DofType::Position)
            ? params.scale
            : 1.0f;

          const auto& kfs = dof.keyframes;
          for (size_t ii = 0; ii < kfs.size(); ++ii)
          {
            anim->addKeyFrame(dof.type, ii * dt, kfs[ii] * mult);
          }
        }
        joint->addChildren(anim);
      }

      // Matrix Order
      {
        using MatrixInstruction = std::function<void(QMatrix4x4&, const QVector3D&)>;
        static MatrixInstruction __instructions[6] =
        {
          [](QMatrix4x4& m, const QVector3D& p) // Position X
          {
            m.translate(QVector3D(p.x(), 0, 0));
          },
          [](QMatrix4x4& m, const QVector3D& p) // Position Y
          {
            m.translate(QVector3D(0, p.y(), 0));
          },
          [](QMatrix4x4& m, const QVector3D& p) // Position Z
          {
            m.translate(QVector3D(0, 0, p.z()));
          },
          [](QMatrix4x4& m, const QVector3D& r) // Rotation X
          {
            m.rotate(r.x(), 1.0, 0.0, 0.0);
          },
          [](QMatrix4x4& m, const QVector3D& r) // Rotation Y
          {
            m.rotate(r.y(), 0.0, 1.0, 0.0);
          },
          [](QMatrix4x4& m, const QVector3D& r) // Rotation Z
          {
            m.rotate(r.z(), 0.0, 0.0, 1.0);
          }
        };

        // Instructions
        std::vector<MatrixInstruction> rotInst, posInst;
        const auto insert = [&](int i, DofType type)
        {
          if (type == DofType::Position) posInst.push_back(__instructions[i]);
          else if (type == DofType::Rotation) rotInst.push_back(__instructions[i + 3]);
        };

        // Construction
        const auto& dofs = jt->dofs;
        const auto find = [&](DofType type)
        {
          const auto pred = [&](AnimationData ad) -> bool
          {
            return ad.type == type;
          };
          return std::find_if(dofs.begin(), dofs.end(), pred);
        };
        const auto construct = [&](DofType type)
        {
          const auto& dof = find(type);
          bool contains = (dof != dofs.end());

          for (int ii = 0; ii < 3; ++ii)
          {
            insert(contains ? dof->ordering[ii] : ii, type);
          }
        };
        construct(DofType::Position);
        construct(DofType::Rotation);

        // Method
        const auto method = [rotInst, posInst](QVector3D pos, QVector3D rot) -> QMatrix4x4
        {
          QMatrix4x4 m = {};
          for (auto inst : posInst) inst(m, pos);
          for (auto inst : rotInst) inst(m, rot);
          return m;
        };
        joint->setMatrixConstruct(method);
      }

      jointComponents.push_back(joint);
    }

    // Construct
    for (size_t ii = 0; ii < joints.size(); ++ii)
    {
      const auto& jtData = joints[ii];
      const auto& jtComp = jointComponents[ii];

      // Root
      if (jtData->parent == nullptr)
      {
        // Copy Content
        result->copy(jtComp);
        jointComponents[ii] = result;
        continue;
      }

      const auto pred = [&](const QSharedPointer<JointData>& jt) -> bool
      {
        return jt.get() == jtData->parent;
      };
      const JointDataVec::iterator& end = joints.begin() + ii;
      const auto& parentJt = std::find_if(joints.begin(), end, pred);

      // Parent not existing
      if (parentJt == end)
      {
        return nullptr;
      }

      size_t parentIndex = std::distance(joints.begin(), parentJt);
      jointComponents[parentIndex]->addChildren(jtComp);
    }
  }

  return result;
}

// ------------------------------------------------------------------------------------------------
FileReader::OFFResult FileReader::readOFF(const QString& filePath, const OFFParameters& params)
{
  auto file = std::ifstream(filePath.toStdString());
  if (!file.is_open())
  {
    return nullptr;
  }

  // Data
  SkinMesh::Vertices vertices(0);
  SkinMesh::Indices indices(0);
  int nVtx, nTri, nEdges;

  // Utilities
  const auto lassert = [](bool cond)
  {
#ifdef _DEBUG
    assert(cond);
#else
    if (!cond) throw false;
#endif
  };
  const auto next = [&](auto& value)
  {
    file >> std::skipws >> value;
  };
  const auto skip = [&]()-> std::string
  {
    std::string val; next(val);
    return val;
  };
  const auto upper = [](std::string s) -> std::string
  {
    std::string r;

    std::transform(s.begin(), s.end(), std::back_inserter(r), [](char c)
                   {
                     return std::toupper(c);
                   });

    return r;
  };

  // Reading
  const auto readHeader = [&]()
  {
    next(nVtx);
    next(nTri);
    next(nEdges);
  };
  const auto readVertices = [&]()
  {
    for (int ii = 0; ii < nVtx; ++ii)
    {
      VertexData_Colored vtx;

      for (int jj = 0; jj < 3; ++jj)
      {
        float v; next(v);
        v *= params.scale;

        vtx.position[jj] = v;
      }
      vtx.color = QVector3D(0.5, 0.5, 0.5);

      vertices.push_back(vtx);
    }
  };
  const auto readTriangles = [&]()
  {
    for (int ii = 0; ii < nTri; ++ii)
    {
      int size; next(size);

      SkinMesh::Poly poly(size);
      for (int jj = 0; jj < size; ++jj)
      {
        int i; next(i);
        poly[jj] = i;
      }

      SkinMesh::ProcessPoly(indices, poly);
    }
  };

  // Algorithm
  try
  {
    lassert(upper(skip()) == "OFF");

    readHeader();
    readVertices();
    readTriangles();
  }
  catch (...)
  {
    return nullptr;
  }

  return OFFResult::create(vertices, indices);
}

// ------------------------------------------------------------------------------------------------
FileReader::WeightResult FileReader::readWeight(const QString& filePath, const WeightParameters& params)
{
  if (params.body.isNull() || params.skin.isNull())
  {
    return nullptr;
  }

  auto file = std::ifstream(filePath.toStdString());
  if (!file.is_open())
  {
    return nullptr;
  }

  // Data
  BodyBase::Hierarchy::Pointer hierarchy = params.body->hierarchy();
  QMap<QString, BodyBase::Hierarchy::Node::Pointer> nodeMap;
  MeshRigRelation::VerticesWeight result(0);
  BodyBase::Hierarchy::Node::Vector nodes(0);
  int nVertices, nNodes;

  // Utilities
  const auto lassert = [](bool cond)
  {
#ifdef _DEBUG
    assert(cond);
#else
    if (!cond) throw false;
#endif
  };
  const auto next = [&](auto& value)
  {
    file >> std::skipws >> value;
  };
  const auto skip = [&]()-> std::string
  {
    std::string val; next(val);
    return val;
  };
  const auto upper = [](std::string s) -> std::string
  {
    std::string r;

    std::transform(s.begin(), s.end(), std::back_inserter(r), [](char c)
                   {
                     return std::toupper(c);
                   });

    return r;
  };

  // Construct
  const auto findVertices = [&]()
  {
    nVertices = (int) params.skin->vSize();
  };
  const auto findJoints = [&]()
  {
    std::vector<BodyBase::Hierarchy::Node::Pointer> nextJoints = {hierarchy->root()}, curr(0);
    int iter = 0;

    // Iterative Search
    while (!nextJoints.empty() && ++iter < 100)
    {
      curr = std::move(nextJoints);
      nextJoints.clear();

      for (const auto& node : curr)
      {
        for (const auto& child : node->children)
        {
          nextJoints.push_back(child);
        }

        nodeMap.insert(node->joint->name(), node);
      }
    }

    nNodes = (int)nodeMap.size();
  };

  // Read
  const auto readJoints = [&]()
  {
    // Read Joint Names
    QVector<QString> names(nNodes);
    {
      lassert(upper(skip()) == "ID");

      for (int ii = 0; ii < nNodes; ++ii)
      {
        std::string name; next(name);
        names[ii] = QString::fromStdString(name);
      }
    }

    // Organize Joint Order
    {
      QVector<BodyBase::Hierarchy::Node::Pointer> orderedNodes(nNodes);

      for (int ii = 0; ii < nNodes; ++ii)
      {
        QString name = names[ii];
        lassert(nodeMap.contains(name));

        orderedNodes[ii] = nodeMap.value(name);
      }

      nodes = std::move(orderedNodes);
    }
  };
  const auto readWeights = [&]()
  {
    static constexpr float eps = 1e-4f;

    result.resize(nVertices);

    for (int ii = 0; ii < nVertices; ++ii)
    {
      // Index
      int index; next(index);
      auto& weights = result[index];

      // Weights
      for (int jj = 0; jj < nNodes; ++jj)
      {
        float weight; next(weight);
        if (weight < eps) continue;

        MeshRigRelation::WeightData data;
        {
          data.node = nodes[jj];
          data.weight = weight;
        }
        weights.push_back(data);
      }
    }
  };

  // Algorithm
  try
  {
    findVertices();
    findJoints();

    readJoints();
    readWeights();
  }
  catch (...)
  {
    return nullptr;
  }

  auto r = WeightResult::create();
  *r = result;
  return r;
}

// ------------------------------------------------------------------------------------------------
FileReader::MTResult FileReader::readMT(const QString& filePath)
{
  auto file = std::ifstream(filePath.toStdString());
  if (!file.is_open())
  {
    return MTResult();
  }

  // Data
  auto result = MTResult();
  int previousPacket = -1;
  //

  // Utilities
  const auto lassert = [](bool cond)
  {
#ifdef _DEBUG
    assert(cond);
#else
    if (!cond) throw false;
#endif
  };
  const auto next = [&](auto& value)
  {
    file >> std::skipws >> value;
  };
  const auto skip = [&]()-> std::string
  {
    std::string val; next(val);
    return val;
  };
  const auto upper = [](std::string s) -> std::string
  {
    std::string r;

    std::transform(s.begin(), s.end(), std::back_inserter(r), [](char c)
                   {
                     return std::toupper(c);
                   });

    return r;
  };

  // Convert
  const auto toEuler = [](const QVector4D& q) -> QVector3D
  {
    float phi = qAtan2(2 * q[2] * q[3] + 2 * q[0] * q[1],
                       2 * q[0] * q[0] + 2 * q[3] * q[3] - 1);

    float theta = -qAsin(2 * q[1] * q[3] - 2 * q[0] * q[2]);

    float psi = qAtan2(2 * q[1] * q[2] + 2 * q[0] * q[3],
                       2 * q[0] * q[0] + 2 * q[1] * q[1] - 1);

    return QVector3D(phi, theta, psi) * RAD2DEG;
  };

  // Construct
  const auto skipHeader = [&]()
  {
    // See to refactor this if need be
    while (skip() != "Quat_q3");
  };
  const auto verifyPacket = [&]() -> bool
  {
    int packet; next(packet);
    if (previousPacket >= 0 && previousPacket + 1 != packet)
      return false;

    previousPacket = packet;
    return true;
  };
  const auto readFrames = [&]()
  {
    int it = 0;
    while (verifyPacket())
    {
      // Skip Acc.
      {
        float _;
        for (int a = 0; a < 3; ++a) next(_);
      }

      // Read Free Acc.
      QVector3D acc;
      {
        float a, b, c;
        next(a);
        next(b);
        next(c);
        acc = {a,b,c};
      }

      // Read Quaternion
      QVector3D rot;
      {
        float a, b, c, d;
        next(a);
        next(b);
        next(c);
        next(d);
        rot = toEuler({a, b, c, d});
      }

      // Add Keyframes
      MTResultFrame frame;
      {
        frame.acceleration = acc;
        frame.rotation = rot;
      }
      result.append(frame);
    }
  };

  // Algorithm
  try
  {
    skipHeader();
    readFrames();
  }
  catch (...)
  {
    return MTResult();
  }

  return result;
}

// ------------------------------------------------------------------------------------------------
FileReader::MTMappingResult FileReader::readMTMapping(const QString& filePath)
{
  auto file = std::ifstream(filePath.toStdString());
  if (!file.is_open())
  {
    return nullptr;
  }

  // Data
  auto result = MTMappingResult::create();
  int trackerCount;
  QString prefix;

  // Utilities
  const auto lassert = [](bool cond)
  {
#ifdef _DEBUG
    assert(cond);
#else
    if (!cond) throw false;
#endif
  };
  const auto next = [&](auto& value)
  {
    file >> std::skipws >> value;
  };
  const auto skip = [&]()-> std::string
  {
    std::string val; next(val);
    return val;
  };
  const auto upper = [](std::string s) -> std::string
  {
    std::string r;

    std::transform(s.begin(), s.end(), std::back_inserter(r), [](char c)
                   {
                     return std::toupper(c);
                   });

    return r;
  };

  // Compute
  const auto computePrefix = [&]()
  {
    QFileInfo fileInfo(filePath);
    QString directory = fileInfo.dir().absolutePath();
    prefix = directory + "/";
  };

  // Read
  const auto readHeader = [&]()
  {
    // Prefix
    {
      std::string name; next(name);
      prefix.append(QString::fromStdString(name));
    }

    // Count
    {
      next(trackerCount);
    }
  };
  const auto readContent = [&]()
  {
    for (int ii = 0; ii < trackerCount; ++ii)
    {
      // Path
      QString path;
      {
        std::string suffix; next(suffix);
        path = prefix + QString::fromStdString(suffix) + ".txt";
      }

      // Name
      QString name;
      {
        std::string v; next(v);
        name = QString::fromStdString(v);
      }

      result->insert(name, path);
    }
  };

  // Algorithm
  try
  {
    computePrefix();

    readHeader();
    readContent();
  }
  catch (...)
  {
    return nullptr;
  }

  return result;
}

// ------------------------------------------------------------------------------------------------
FileReader::MTSkinResult FileReader::readMTSkin(const QString& filePath, const MTSkinParameters& params)
{
  auto file = std::ifstream(filePath.toStdString());
  if (!file.is_open())
  {
    return nullptr;
  }

  // Data
  MTSkinMesh::Vertices vertices(0);
  MTSkinMesh::Indices indices(0);
  // ---
  using Position = std::array<float, 3>;
  std::vector<Position> positions(0);
  // ---
  using Normal = std::array<float, 3>;
  std::vector<Normal> normals(0);
  // ---
  using TexCoord = std::array<float, 2>;
  std::vector<TexCoord> texCoords(0);
  // ---
  using Face = std::vector<std::string>;
  std::vector<Face> faces(0);

  // Utilities
  const auto lassert = [](bool cond)
  {
#ifdef _DEBUG
    assert(cond);
#else
    if (!cond) throw false;
#endif
  };
  const auto next = [&](auto& value)
  {
    file >> std::skipws >> value;
  };
  const auto skip = [&]()-> std::string
  {
    std::string val; next(val);
    return val;
  };
  const auto upper = [](std::string s) -> std::string
  {
    std::string r;

    std::transform(s.begin(), s.end(), std::back_inserter(r), [](char c)
      {
        return std::toupper(c);
      });

    return r;
  };

  // Reading
  const auto readPosition = [&]()
  {
    Position p;
    next(p[0]);
    next(p[1]);
    next(p[2]);
    positions.push_back(p);
  };
  const auto readNormal = [&]()
  {
    Normal n;
    next(n[0]);
    next(n[1]);
    next(n[2]);
    normals.push_back(n);
  };
  const auto readTexCoord = [&]()
  {
    TexCoord t;
    next(t[0]);
    next(t[1]);
    texCoords.push_back(t);
  };
  const auto readFace = [&]()
  {
    const size_t faceSize = 3;

    Face f(faceSize);
    for (size_t ii = 0; ii < faceSize; ++ii)
    {
      next(f[ii]);
    }
    faces.push_back(f);
  };
  const auto readToken = [&]() -> bool
  {
    std::string token = upper(skip());

    if      (token == "V")  readPosition();
    else if (token == "VN") readNormal();
    else if (token == "VT") readTexCoord();
    else if (token == "F")  readFace();

    else return false;
    return true;
  };

  // Convert
  const auto convertFaces = [&]()
  {
    bool hasNormals = !normals.empty();
    bool hasTexCoords = !texCoords.empty();

    for (const auto& face : faces)
    {
      MTSkinMesh::Poly poly(0);
      
      for (auto it : face)
      {
        VertexData_Textured vtx;
        {
          int nSep = std::count(it.begin(), it.end(), '/');
          
          if (!(
            (!hasNormals && nSep == 1)
            || (nSep == 2)
            ))
          {
            continue;
          }

          std::replace(it.begin(), it.end(), '/', ' ');
          std::stringstream ss(it);
          {
            // Position
            {
              int pIndex;
              ss >> std::skipws >> pIndex;
              const auto& p = positions[pIndex - 1];

              vtx.position = params.transform * QVector3D(p[0], p[1], p[2]);
            }

            // Texture
            if (hasTexCoords)
            {
              int tIndex;
              ss >> std::skipws >> tIndex;
              const auto& t = texCoords[tIndex - 1];

              vtx.texCoord = QVector2D(t[0], t[1]);
            }

            // Normal
            if (hasNormals)
            {
              int nIndex;
              ss >> std::skipws >> nIndex;
              const auto& n = normals[nIndex - 1];

              vtx.normal = QVector3D(n[0], n[1], n[2]);
            }
          }
        }

        poly.push_back((GLushort)vertices.size());
        vertices.push_back(vtx);
      }

      MTSkinMesh::ProcessPoly(indices, poly);
    }
  };

  // Algorithm
  try
  {
    while (readToken());

    convertFaces();
  }
  catch (...)
  {
    return nullptr;
  }

  return MTSkinResult::create(params.texture, vertices, indices);
}

// ------------------------------------------------------------------------------------------------
FileReader::TransformResult FileReader::readTransform(const QString& filePath)
{
  auto file = std::ifstream(filePath.toStdString());
  if (!file.is_open())
  {
    return QMatrix4x4();
  }

  // Data
  QVector3D pos, rot, scale;

  // Utilities
  const auto lassert = [](bool cond)
  {
#ifdef _DEBUG
    assert(cond);
#else
    if (!cond) throw false;
#endif
  };
  const auto next = [&](auto& value)
  {
    file >> std::skipws >> value;
  };
  const auto skip = [&]()-> std::string
  {
    std::string val; next(val);
    return val;
  };
  const auto upper = [](std::string s) -> std::string
  {
    std::string r;

    std::transform(s.begin(), s.end(), std::back_inserter(r), [](char c)
      {
        return std::toupper(c);
      });

    return r;
  };

  // Reading
  const auto readTranslate = [&]()
  {
    next(pos[0]);
    next(pos[1]);
    next(pos[2]);
  };
  const auto readRotation = [&]()
  {
    next(rot[0]);
    next(rot[1]);
    next(rot[2]);
  };
  const auto readScale = [&]()
  {
    next(scale[0]);
    next(scale[1]);
    next(scale[2]);
  };

  // Convert
  const auto convertTransform = [&]() -> QMatrix4x4
  {
    QMatrix4x4 t;
    t.translate(pos);

    QMatrix4x4 r;
    r.rotate(rot.x(), 1.0, 0.0, 0.0);
    r.rotate(rot.y(), 0.0, 1.0, 0.0);
    r.rotate(rot.z(), 0.0, 0.0, 1.0);

    QMatrix4x4 s;
    for (int ii = 0; ii < 3; ++ii)
      s(ii, ii) = scale[ii];

    return t * r * s;
  };

  // Algorithm
  try
  {
    lassert(upper(skip()) == "T");
    readTranslate();

    lassert(upper(skip()) == "R");
    readRotation();

    lassert(upper(skip()) == "S");
    readScale();
  }
  catch (...)
  {
    return QMatrix4x4();
  }

  return convertTransform();
}
