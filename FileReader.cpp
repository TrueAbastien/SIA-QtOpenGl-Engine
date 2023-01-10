#include "FileReader.h"

#include "AnimatorPlug.h"

#include <QRandomGenerator>
#include <QFileInfo>
#include <QDir>

#include <fstream>
#include <functional>
#include <algorithm>
#include <numeric>

// ------------------------------------------------------------------------------------------------
using DofType = AnimatorPlug::PropertyType;
struct AnimationData
{
  std::vector<float> keyframes;
  DofType type;
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

      int n; next(n);
      joint->dofs = std::vector<AnimationData>(n);

      for (int ii = 0; ii < n; ++ii)
      {
        std::string symbol; next(symbol);
        symbol = upper(symbol);

        AnimationData& anim = joint->dofs[ii];

        // Position
        if (symbol == "XPOSITION") anim.type = DofType::PositionX;
        else if (symbol == "YPOSITION") anim.type = DofType::PositionY;
        else if (symbol == "ZPOSITION") anim.type = DofType::PositionZ;

        // Rotation
        else if (symbol == "XROTATION") anim.type = DofType::RotationX;
        else if (symbol == "YROTATION") anim.type = DofType::RotationY;
        else if (symbol == "ZROTATION") anim.type = DofType::RotationZ;

        // Error
        else lassert(false);
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
        auto& anim = jt->dofs;

        for (size_t jj = 0; jj < anim.size(); ++jj)
        {
          float val; next(val);
          anim[jj].keyframes.push_back(val);
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
          float mult = ((int) dof.type < (int) DofType::RotationX)
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

        // Indexors
        std::vector<int> indexor(6);
        std::iota(indexor.begin(), indexor.end(), 0);

        // Instructions
        std::vector<MatrixInstruction> rotInst, posInst;
        const auto insert = [&](int i)
        {
          if (i < 3) posInst.push_back(__instructions[i]);
          else rotInst.push_back(__instructions[i]);
        };
        for (const auto& dof : jt->dofs)
        {
          int index = (int) dof.type;
          indexor.erase(std::remove(indexor.begin(), indexor.end(), index), indexor.end());

          insert(index);
        }
        for (const auto& index : indexor)
        {
          insert(index);
        }

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
  if (params.root.isNull() || params.skin.isNull())
  {
    return nullptr;
  }

  auto file = std::ifstream(filePath.toStdString());
  if (!file.is_open())
  {
    return nullptr;
  }

  // Data
  QMap<QString, Component::Pointer> jointMap;
  MeshRigRelation::VerticesWeight result(0);
  QVector<Component::Pointer> joints(0);
  int nVertices, nJoints;

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
    std::vector<Component::Pointer> nextJoints = {params.root}, curr(0);
    int iter = 0;

    // Iterative Search
    while (!nextJoints.empty() && ++iter < 100)
    {
      curr = std::move(nextJoints);
      nextJoints.clear();

      for (const auto& jt : curr)
      {
        for (const auto& child : jt->children())
        {
          if (!child.dynamicCast<Joint>().isNull())
          {
            nextJoints.push_back(child);
          }
        }

        jointMap.insert(jt->name(), jt);
      }
    }

    nJoints = (int) jointMap.size();
  };

  // Read
  const auto readJoints = [&]()
  {
    // Read Joint Names
    QVector<QString> names(nJoints);
    {
      lassert(upper(skip()) == "ID");

      for (int ii = 0; ii < nJoints; ++ii)
      {
        std::string name; next(name);
        names[ii] = QString::fromStdString(name);
      }
    }

    // Organize Joint Order
    {
      QVector<Component::Pointer> orderedJoints(nJoints);

      for (int ii = 0; ii < nJoints; ++ii)
      {
        QString name = names[ii];
        lassert(jointMap.contains(name));

        orderedJoints[ii] = jointMap.value(name);
      }

      joints = std::move(orderedJoints);
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
      for (int jj = 0; jj < nJoints; ++jj)
      {
        float weight; next(weight);
        if (weight < eps) continue;

        MeshRigRelation::WeightData data;
        {
          data.joint = joints[jj];
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
FileReader::MTResult FileReader::readMT(const QString& filePath, const MTParameters& params)
{
  if (params.parent.isNull())
  {
    return nullptr;
  }

  auto file = std::ifstream(filePath.toStdString());
  if (!file.is_open())
  {
    return nullptr;
  }

  // Data
  auto result = MTResult::create();
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
  const auto quaternionToEuler = [](QVector4D quat) -> QVector3D
  {
    float x, y, z;
    QQuaternion q(quat);
    q.getEulerAngles(&x, &y, &z);
    return QVector3D(x, y, z);
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
    static const float dt = 1.0f / params.samplingRate;

    int it = 0;
    while (verifyPacket())
    {
      // Skip All Acc.
      {
        float _;
        for (int a = 0; a < 6; ++a) next(_);
      }

      // Read Quaternion
      QVector4D quat;
      {
        float a, b, c, d;
        next(a);
        next(b);
        next(c);
        next(d);
        quat = {a,b,c,d};
      }
      QVector3D rot = quaternionToEuler(quat);

      // Add Keyframes
      float time = dt * (++it);
      /*for (int ii = 0; ii < 3; ++ii)
      {
        anim->addKeyFrame((AnimatorPlug::PropertyType) ii, time, acc[ii]);
      }*/
      for (int jj = 0; jj < 3; ++jj)
      {
        result->addKeyFrame((AnimatorPlug::PropertyType) (jj + 3), time, rot[jj]);
      }
    }

    // Setup Parenting
    params.parent->addChildren(result);

    // Set Matrix Construction
    if (params.overrideMatrixMethod)
    {
      const auto method = [](QVector3D pos, QVector3D rot) -> QMatrix4x4
      {
        QMatrix4x4 m = {};
        m.translate(pos);
        m.rotate(rot.x(), 1.0, 0.0, 0.0);
        m.rotate(rot.y(), 0.0, 1.0, 0.0);
        m.rotate(rot.z(), 0.0, 0.0, 1.0);
        return m;
      };
      params.parent->setMatrixConstruct(method);
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
    return nullptr;
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
