#include "MTSetupWindow.h"

#include "AxisCorrector.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFileDialog>


// ------------------------------------------------------------------------------------------------
MTSetupWindow::MTSetupWindow(QWidget* parent) :
  QDialog(parent)
{
  setWindowTitle("MT Setup");

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
        //connect(this, &MeshEditWindow::meshChanged, edit, &QLineEdit::setText);
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
            //connect(this, &MeshEditWindow::positionXChanged, spinBox, &QDoubleSpinBox::setValue);
            //connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updatePositionX(double)));
            edit->addWidget(spinBox, 0, 1);
          }

          // Y
          {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox;
            spinBox->setValue(0.0);
            spinBox->setMaximum(100.0);
            spinBox->setMinimum(-100.0);
            spinBox->setSingleStep(1.0);
            //connect(this, &MeshEditWindow::positionYChanged, spinBox, &QDoubleSpinBox::setValue);
            //connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updatePositionY(double)));
            edit->addWidget(spinBox, 0, 2);
          }

          // Z
          {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox;
            spinBox->setValue(0.0);
            spinBox->setMaximum(100.0);
            spinBox->setMinimum(-100.0);
            spinBox->setSingleStep(1.0);
            //connect(this, &MeshEditWindow::positionZChanged, spinBox, &QDoubleSpinBox::setValue);
            //connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updatePositionZ(double)));
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
            //connect(this, &MeshEditWindow::rotationXChanged, spinBox, &QDoubleSpinBox::setValue);
            //connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updateRotationX(double)));
            edit->addWidget(spinBox, 1, 1);
          }

          // Y
          {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox;
            spinBox->setValue(0.0);
            spinBox->setMaximum(180.0);
            spinBox->setMinimum(-180.0);
            spinBox->setSingleStep(5.0);
            //connect(this, &MeshEditWindow::rotationYChanged, spinBox, &QDoubleSpinBox::setValue);
            //connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updateRotationY(double)));
            edit->addWidget(spinBox, 1, 2);
          }

          // Z
          {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox;
            spinBox->setValue(0.0);
            spinBox->setMaximum(180.0);
            spinBox->setMinimum(-180.0);
            spinBox->setSingleStep(5.0);
            //connect(this, &MeshEditWindow::rotationZChanged, spinBox, &QDoubleSpinBox::setValue);
            //connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updateRotationZ(double)));
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
          //connect(this, &MeshEditWindow::scaleChanged, spinBox, &QDoubleSpinBox::setValue);
          //connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(updateScale(double)));
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
        //connect(button, &QPushButton::pressed, this, &MeshEditWindow::save);
        //connect(this, &MeshEditWindow::unedited, button, &QPushButton::setEnabled);
        buttons->addWidget(button);
      }

      // Apply
      {
        QPushButton* button = new QPushButton("Apply");
        //connect(button, &QPushButton::pressed, this, &MeshEditWindow::apply);
        buttons->addWidget(button);
      }

      // Cancel
      {
        QPushButton* button = new QPushButton("Cancel");
        //connect(button, &QPushButton::pressed, this, &MeshEditWindow::cancel);
        buttons->addWidget(button);
      }
    }
  }
}

// ------------------------------------------------------------------------------------------------
void MTSetupWindow::setBody(const Body& body)
{
  m_body = body;
}

// ------------------------------------------------------------------------------------------------
void MTSetupWindow::loadTracker()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open MT"), "", tr("MT Files (*.txt)"));
  if (fileName.isEmpty())
  {
    return;
  }

  // TODO: find 'limb' to attach to

  FileReader::MTParameters params;
  {
    params.parent = nullptr; // TODO
    params.samplingRate = 60;
  }

  auto result = FileReader::readMT(fileName, params);
  if (result == nullptr)
  {
    return;
  }

  // TODO: add & list trackers
}
