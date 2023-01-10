#pragma once

#include "Structs.h"
#include "FileReader.h"

#include <QDialog>
#include <QCloseEvent>

class MTSetupWindow : public QDialog
{
  Q_OBJECT;

public:

  using Tracker = FileReader::MTResult;
  using TrackerVec = QVector<Tracker>;

  using Body = FileReader::BVHResult;

public:

  MTSetupWindow(QWidget* parent);

public:

  void setBody(const Body& body);

public slots:

  void loadTracker();

signals:

  void bodyChanged(const QString&);

private:

  Body m_body;
  TrackerVec m_trackers;
};
