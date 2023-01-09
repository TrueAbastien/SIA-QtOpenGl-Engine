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

public:

  MTSetupWindow(QWidget* parent);

public slots:

  void loadTracker();

private:

  void addTracker(const Tracker& tracker);

  QStringList trackerList(int excluding) const;

private:

  QLayout* m_listingLayout = nullptr;
  TrackerVec m_trackers;
};
