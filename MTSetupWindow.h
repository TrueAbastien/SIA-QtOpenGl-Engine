#pragma once

#include "Structs.h"
#include "FileReader.h"

#include <QDialog>
#include <QCloseEvent>

class MTSetupWindow : public QDialog
{
  Q_OBJECT;

public:

  using Body = FileReader::BVHResult;
  using JointMap = QMap<QString, Component::Pointer>;

public:

  MTSetupWindow(QWidget* parent, LogMethod logMethod);

public:

  void setBody(const Body& body);

public slots:

  void loadMapping();

signals:

  void bodyChanged(const QString&);

private:

  LogMethod log;

private:

  Body m_body;
};
