#pragma once

#include <QVector>
#include <QWidget>

class QCheckBox;
class QLabel;
class QSlider;

class MotorControlWidget final : public QWidget {
  Q_OBJECT

 public:
  explicit MotorControlWidget(QWidget* parent = nullptr);

 private slots:
  void onUnlockToggled(bool enabled);
  void onThreeDModeToggled(bool enabled);
  void onMasterSliderChanged(int value);
  void onIndividualSliderChanged(int index, int value);

 private:
  struct MotorRow {
    QLabel* title{nullptr};
    QSlider* slider{nullptr};
    QLabel* value{nullptr};
  };

  void applyRange();
  void resetAllToIdle();
  void syncMasterFromIndividuals();
  void updateLabels();

  int minThrottle_{1000};
  int maxThrottle_{2000};
  int idleThrottle_{1000};

  QCheckBox* unlockCheck_{nullptr};
  QCheckBox* threeDModeCheck_{nullptr};
  QLabel* rangeLabel_{nullptr};
  QSlider* masterSlider_{nullptr};
  QLabel* masterValueLabel_{nullptr};
  QVector<MotorRow> motorRows_;
};
