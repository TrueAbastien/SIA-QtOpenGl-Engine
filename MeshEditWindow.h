
#pragma once

#include "Structs.h"
#include "SkinMesh.h"

#include <QWidget>

class MeshEditWindow final : public QWidget
{
  Q_OBJECT;

public:

  using MeshPtr = QSharedPointer<SkinMesh>;

public:

  MeshEditWindow(QWidget* parent);

public:

  void setSkinMesh(const MeshPtr& mesh);

public slots:

  void updateScale(double value);

  void updateRotationX(double value);
  void updateRotationY(double value);
  void updateRotationZ(double value);

  void updatePositionX(double value);
  void updatePositionY(double value);
  void updatePositionZ(double value);

  void save();
  void apply();
  void cancel();

signals:

  void scaleChanged(double value);

  void rotationXChanged(double value);
  void rotationYChanged(double value);
  void rotationZChanged(double value);

  void positionXChanged(double value);
  void positionYChanged(double value);
  void positionZChanged(double value);

  void meshChanged(const QString&);
  void unedited(bool);

private:

  void resetTransform();

private:

  MeshPtr m_mesh;

  float m_scale, m_savedScale;
  QVector3D m_rotation, m_savedRotation;
  QVector3D m_position, m_savedPosition;
};