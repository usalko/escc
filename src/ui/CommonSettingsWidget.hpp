#pragma once

#include <QWidget>

class QLabel;

class CommonSettingsWidget final : public QWidget {
  Q_OBJECT

 public:
  explicit CommonSettingsWidget(QWidget* parent = nullptr);

 private:
  static QString sliderValueText(int value);

  QLabel* startupPowerValueLabel_;
};
