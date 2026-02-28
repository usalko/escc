#pragma once

#include <QWidget>

class HomeWidget final : public QWidget {
  Q_OBJECT

 public:
  explicit HomeWidget(QWidget* parent = nullptr);

 signals:
  void connectRequested();
  void disconnectRequested();
  void readSettingsRequested();
  void flashFirmwareRequested();
};
