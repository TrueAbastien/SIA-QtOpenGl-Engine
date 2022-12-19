#include "MeshEditWindow.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>

// ------------------------------------------------------------------------------------------------
MeshEditWindow::MeshEditWindow(QWidget* parent) :
  QDialog(parent),
  m_scale(1.0f), m_savedScale(1.0f),
  m_rotation{}, m_savedRotation{},
  m_position{}, m_savedPosition{}
{
  setWindowTitle("Mesh Edit");

  // Content
  {
    QVBoxLayout* root = new QVBoxLayout;
    this->setLayout(root);

    // Name
    {
      QHBoxLayout* name = new QHBoxLayout;
      root->addLayout(name);

      // Label
      {
        QLabel* label = new QLabel("Name");
        name->addWidget(label);
      }

      // Name
      {
        QLineEdit* edit = new QLineEdit("");
        edit->setReadOnly(true);
        connect(this, &MeshEditWindow::meshChanged, edit, &QLineEdit::setText);
        name->addWidget(edit);
      }
    }

    // Edit
    {
      QGridLayout* edit = new QGridLayout;
      root->addLayout(edit);

      // Position
      {
        // Label
        {
          QLabel* label = new QLabel("Position");
          edit->addWidget(label, 0, 0);
        }

        // Values
        {
          // X
          {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox;
            spinBox->setValue(0.0);
            spinBox->setMaximum(100.0);
            spinBox->setMinimum(-100.0);
            spinBox->setSingleStep(1.0);
            connect(this, &MeshEditWindow::positionXChanged, spinBox, &QDoubleSpinBox::setValue);
            connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updatePositionX(double)));
            edit->addWidget(spinBox, 0, 1);
          }

          // Y
          {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox;
            spinBox->setValue(0.0);
            spinBox->setMaximum(100.0);
            spinBox->setMinimum(-100.0);
            spinBox->setSingleStep(1.0);
            connect(this, &MeshEditWindow::positionYChanged, spinBox, &QDoubleSpinBox::setValue);
            connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updatePositionY(double)));
            edit->addWidget(spinBox, 0, 2);
          }

          // Z
          {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox;
            spinBox->setValue(0.0);
            spinBox->setMaximum(100.0);
            spinBox->setMinimum(-100.0);
            spinBox->setSingleStep(1.0);
            connect(this, &MeshEditWindow::positionZChanged, spinBox, &QDoubleSpinBox::setValue);
            connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updatePositionZ(double)));
            edit->addWidget(spinBox, 0, 3);
          }
        }
      }

      // Rotation
      {
        // Label
        {
          QLabel* label = new QLabel("Rotation");
          edit->addWidget(label, 1, 0);
        }

        // Values
        {
          // X
          {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox;
            spinBox->setValue(0.0);
            spinBox->setMaximum(180.0);
            spinBox->setMinimum(-180.0);
            spinBox->setSingleStep(5.0);
            connect(this, &MeshEditWindow::rotationXChanged, spinBox, &QDoubleSpinBox::setValue);
            connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updateRotationX(double)));
            edit->addWidget(spinBox, 1, 1);
          }

          // Y
          {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox;
            spinBox->setValue(0.0);
            spinBox->setMaximum(180.0);
            spinBox->setMinimum(-180.0);
            spinBox->setSingleStep(5.0);
            connect(this, &MeshEditWindow::rotationYChanged, spinBox, &QDoubleSpinBox::setValue);
            connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updateRotationY(double)));
            edit->addWidget(spinBox, 1, 2);
          }

          // Z
          {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox;
            spinBox->setValue(0.0);
            spinBox->setMaximum(180.0);
            spinBox->setMinimum(-180.0);
            spinBox->setSingleStep(5.0);
            connect(this, &MeshEditWindow::rotationZChanged, spinBox, &QDoubleSpinBox::setValue);
            connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updateRotationZ(double)));
            edit->addWidget(spinBox, 1, 3);
          }
        }
      }

      // Scale
      {
        // Label
        {
          QLabel* label = new QLabel("Scale");
          edit->addWidget(label, 2, 0);
        }

        // Value
        {
          QDoubleSpinBox* spinBox = new QDoubleSpinBox;
          spinBox->setValue(1.0);
          spinBox->setDecimals(4);
          spinBox->setMaximum(10.0);
          spinBox->setMinimum(0.0);
          spinBox->setSingleStep(0.1);
          connect(this, &MeshEditWindow::scaleChanged, spinBox, &QDoubleSpinBox::setValue);
          connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updateScale(double)));
          edit->addWidget(spinBox, 2, 1, 1, 3);
        }
      }
    }

    // Buttons
    {
      QHBoxLayout* buttons = new QHBoxLayout;
      root->addLayout(buttons);

      // Save
      {
        QPushButton* button = new QPushButton("Save");
        connect(button, &QPushButton::pressed, this, &MeshEditWindow::save);
        connect(this, &MeshEditWindow::unedited, button, &QPushButton::setEnabled);
        buttons->addWidget(button);
      }

      // Apply
      {
        QPushButton* button = new QPushButton("Apply");
        connect(button, &QPushButton::pressed, this, &MeshEditWindow::apply);
        buttons->addWidget(button);
      }

      // Cancel
      {
        QPushButton* button = new QPushButton("Cancel");
        connect(button, &QPushButton::pressed, this, &MeshEditWindow::cancel);
        buttons->addWidget(button);
      }
    }
  }

  // Connections
  {
    // TODO
  }
}

// ------------------------------------------------------------------------------------------------
void MeshEditWindow::setSkinMesh(const MeshPtr& mesh)
{
  // Revert previous Mesh
  if (!m_mesh.isNull()) cancel();

  m_mesh = mesh;

  resetTransform();

  emit meshChanged(mesh->parent()->name());
}

// ------------------------------------------------------------------------------------------------
void MeshEditWindow::closeEvent(QCloseEvent* e)
{
  cancel();

  m_mesh = nullptr;
}

// ------------------------------------------------------------------------------------------------
void MeshEditWindow::updateScale(double value)
{
  m_scale = (float) value;

  if (m_mesh.isNull()) return;
  m_mesh->setScale(m_scale);
  emit unedited(false);
}

// ------------------------------------------------------------------------------------------------
void MeshEditWindow::updateRotationX(double value)
{
  m_rotation.setX((float) value);

  if (m_mesh.isNull()) return;
  m_mesh->setLocalRotation(m_rotation);
  emit unedited(false);
}

// ------------------------------------------------------------------------------------------------
void MeshEditWindow::updateRotationY(double value)
{
  m_rotation.setY((float) value);

  if (m_mesh.isNull()) return;
  m_mesh->setLocalRotation(m_rotation);
  emit unedited(false);
}

// ------------------------------------------------------------------------------------------------
void MeshEditWindow::updateRotationZ(double value)
{
  m_rotation.setZ((float) value);

  if (m_mesh.isNull()) return;
  m_mesh->setLocalRotation(m_rotation);
  emit unedited(false);
}

// ------------------------------------------------------------------------------------------------
void MeshEditWindow::updatePositionX(double value)
{
  m_position.setX((float) value);

  if (m_mesh.isNull()) return;
  m_mesh->setLocalPosition(m_position);
  emit unedited(false);
}

// ------------------------------------------------------------------------------------------------
void MeshEditWindow::updatePositionY(double value)
{
  m_position.setY((float) value);

  if (m_mesh.isNull()) return;
  m_mesh->setLocalPosition(m_position);
  emit unedited(false);
}

// ------------------------------------------------------------------------------------------------
void MeshEditWindow::updatePositionZ(double value)
{
  m_position.setZ((float) value);

  if (m_mesh.isNull()) return;
  m_mesh->setLocalPosition(m_position);
  emit unedited(false);
}

// ------------------------------------------------------------------------------------------------
void MeshEditWindow::save()
{
  // TODO
}

// ------------------------------------------------------------------------------------------------
void MeshEditWindow::apply()
{
  if (m_mesh.isNull()) return;

  auto& vertices = m_mesh->vertices();
  QMatrix4x4 tr = m_mesh->localToParent();

  for (auto& vertex : vertices)
  {
    vertex.position = tr * vertex.position;
  }
  m_mesh->verticesUpdate();

  m_mesh->setScale(1.0f);
  m_mesh->setLocalRotation(QVector3D());
  m_mesh->setLocalPosition(QVector3D());

  resetTransform();

  emit unedited(true);
}

// ------------------------------------------------------------------------------------------------
void MeshEditWindow::cancel()
{
  if (m_mesh.isNull()) return;

  m_mesh->setScale(m_savedScale);
  m_mesh->setLocalRotation(m_savedRotation);
  m_mesh->setLocalPosition(m_savedPosition);

  resetTransform();
}

// ------------------------------------------------------------------------------------------------
void MeshEditWindow::resetTransform()
{
  if (m_mesh.isNull()) return;

  m_scale = m_mesh->scale();
  m_rotation = m_mesh->localRotation();
  m_position = m_mesh->localPosition();

  m_savedScale = m_scale;
  m_savedRotation = m_rotation;
  m_savedPosition = m_position;

  emit scaleChanged((double) m_scale);

  emit rotationXChanged((double) m_rotation.x());
  emit rotationYChanged((double) m_rotation.y());
  emit rotationZChanged((double) m_rotation.z());

  emit positionXChanged((double) m_position.x());
  emit positionYChanged((double) m_position.y());
  emit positionZChanged((double) m_position.z());

  emit unedited(true);
}
