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
void fillJointMap(MTSetupWindow::JointMap& joints, const Component::Pointer& parent)
{
  if (parent.dynamicCast<Joint>().isNull() && parent.dynamicCast<JointRenderer>().isNull())
  {
    return;
  }

  QString name = parent->name();
  if (joints.contains(name))
  {
    return;
  }

  joints.insert(name, parent);

  for (const auto& child : parent->children())
  {
    fillJointMap(joints, child);
  }
}

// ------------------------------------------------------------------------------------------------
MTSetupWindow::MTSetupWindow(QWidget* parent, LogMethod logMethod) :
  QDialog(parent),
  log(logMethod)
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

    // Mapping
    {
      QHBoxLayout* mapping = new QHBoxLayout;
      root->addLayout(mapping);

      // Label
      {
        QLabel* label = new QLabel("Mapping");
        mapping->addWidget(label);
      }

      // Name
      {
        QPushButton* button = new QPushButton("Load");
        connect(button, &QPushButton::pressed, this, &MTSetupWindow::loadMapping);
        mapping->addWidget(button);
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
void MTSetupWindow::loadMapping()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open Mapping"), "", tr("Mapping Files (*.txt)"));
  if (fileName.isEmpty())
  {
    return;
  }

  auto mapping = FileReader::readMTMapping(fileName);
  if (mapping.isNull())
  {
    log(LogType::ERROR_, "Couldn't read Mapping in '" + fileName.toStdString() + "'...");
    return;
  }

  log(LogType::INFO, "'" + fileName.toStdString() + "' read successfully !");

  JointMap joints;
  fillJointMap(joints, m_body);

  for (const QString& name : joints.keys())
  {
    QString path = mapping->value(name, "");
    if (path.isEmpty())
    {
      continue;
    }

    FileReader::MTParameters params;
    {
      params.parent = joints[name];
      params.samplingRate = 60;
    }

    auto result = FileReader::readMT(path, params);
    if (result == nullptr)
    {
      log(LogType::ERROR_, "Couldn't read Tracker for '" + name.toStdString() + "' in '" + path.toStdString() + "'...");
      return;
    }

    result->init();

    log(LogType::INFO, "'" + path.toStdString() + "' for '" + name.toStdString() + "' read successfully !");
  }

  log(LogType::INFO, "Mapping finished !");
}
