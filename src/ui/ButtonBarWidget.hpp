#pragma once

#include <QWidget>

class ButtonBarWidget final : public QWidget {
  Q_OBJECT

 public:
  explicit ButtonBarWidget(QWidget* parent = nullptr);

 signals:
  void readSettingsRequested();
  void writeSettingsRequested();
  void flashAllRequested();
  void selectFirmwareRequested();
};
