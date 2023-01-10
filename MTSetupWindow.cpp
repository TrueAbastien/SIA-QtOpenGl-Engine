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
        connect(this, &MTSetupWindow::bodyChanged, edit, &QLineEdit::setText);
        name->addWidget(edit);
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
  if (body.isNull()) return;

  m_body = body;

  emit bodyChanged(body->parent()->name());
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
