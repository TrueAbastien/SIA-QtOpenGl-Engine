#pragma once

#include "Structs.h"
#include "FileReader.h"
#include "MTBody.h"
#include "Scene.h"

#include <QDialog>
#include <QCloseEvent>

class MTSetupWindow : public QDialog
{
  Q_OBJECT;

public:

  using BodyReference = FileReader::BVHResult;
  using BodyTracked = QSharedPointer<MTBody>;

  using JointMap = QMap<QString, Component::Pointer>;

public:

  MTSetupWindow(QWidget* parent, LogMethod logMethod);

public:

  void setBody(const BodyReference& body);

public slots:

  void loadMapping();

signals:

  void bodyChanged(const QString&);

private:

  LogMethod log;

private:

  BodyTracked m_body;
};
