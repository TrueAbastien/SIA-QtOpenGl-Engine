#include "FileReader.h"

#include "AnimatorPlug.h"

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

    lassert(upper(token) == "MOTION");
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
      // TODO: use name ?
      joint->setLocalPosition(jt->offset * params.scale);

      // Animation
      auto anim = QSharedPointer<AnimatorPlug>::create();
      for (const auto& dof : jt->dofs)
      {
        float mult = ((int) dof.type < (int) DofType::RotationX)
          ? params.scale
          : 1.0f;

        const auto& kfs = dof.keyframes;
        for (size_t ii = 0; ii < kfs.size(); ++ii)
        {
          // TODO: verify offset in Position affixment
          anim->addKeyFrame(dof.type, ii * dt, kfs[ii] * mult);
        }
      }
      joint->addChildren(anim);

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
        result->merge(jtComp);
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
