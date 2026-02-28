#include "MotorControlWidget.hpp"

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSignalBlocker>
#include <QSlider>
#include <QVBoxLayout>

MotorControlWidget::MotorControlWidget(QWidget* parent) : QWidget(parent) {
  setObjectName(QStringLiteral("motorControlWidget"));

  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(16, 16, 16, 16);
  root->setSpacing(10);

  auto* title = new QLabel(tr("Motor Control"), this);
  title->setObjectName(QStringLiteral("homeTitle"));

  auto* subtitle = new QLabel(
      tr("Use sliders to test motor response. Remove propellers before enabling motor output."),
      this);
  subtitle->setWordWrap(true);

  unlockCheck_ = new QCheckBox(tr("Enable motor control"), this);
  threeDModeCheck_ = new QCheckBox(tr("Enable 3D mode"), this);
  rangeLabel_ = new QLabel(this);

  auto* toggles = new QHBoxLayout();
  toggles->setSpacing(16);
  toggles->addWidget(unlockCheck_);
  toggles->addWidget(threeDModeCheck_);
  toggles->addStretch(1);

  auto* masterGroup = new QGroupBox(tr("Master Speed"), this);
  auto* masterLayout = new QHBoxLayout(masterGroup);
  masterSlider_ = new QSlider(Qt::Horizontal, masterGroup);
  masterSlider_->setSingleStep(10);
  masterSlider_->setPageStep(50);
  masterValueLabel_ = new QLabel(masterGroup);
  masterValueLabel_->setMinimumWidth(60);
  masterLayout->addWidget(masterSlider_, 1);
  masterLayout->addWidget(masterValueLabel_);

  auto* motorsGroup = new QGroupBox(tr("Individual Motors"), this);
  auto* motorsLayout = new QGridLayout(motorsGroup);
  motorsLayout->setHorizontalSpacing(12);
  motorsLayout->setVerticalSpacing(8);

  for (int i = 0; i < 4; ++i) {
    MotorRow row;
    row.title = new QLabel(tr("Motor %1").arg(i + 1), motorsGroup);
    row.slider = new QSlider(Qt::Horizontal, motorsGroup);
    row.slider->setSingleStep(10);
    row.slider->setPageStep(50);
    row.value = new QLabel(motorsGroup);
    row.value->setMinimumWidth(60);

    motorsLayout->addWidget(row.title, i, 0);
    motorsLayout->addWidget(row.slider, i, 1);
    motorsLayout->addWidget(row.value, i, 2);

    connect(row.slider, &QSlider::valueChanged, this, [this, i](int value) {
      onIndividualSliderChanged(i, value);
    });

    motorRows_.push_back(row);
  }

  root->addWidget(title);
  root->addWidget(subtitle);
  root->addLayout(toggles);
  root->addWidget(rangeLabel_);
  root->addWidget(masterGroup);
  root->addWidget(motorsGroup);
  root->addStretch(1);

  connect(unlockCheck_, &QCheckBox::toggled, this, &MotorControlWidget::onUnlockToggled);
  connect(threeDModeCheck_, &QCheckBox::toggled, this, &MotorControlWidget::onThreeDModeToggled);
  connect(masterSlider_, &QSlider::valueChanged, this, &MotorControlWidget::onMasterSliderChanged);

  applyRange();
  resetAllToIdle();
  onUnlockToggled(false);
}

void MotorControlWidget::onUnlockToggled(bool enabled) {
  masterSlider_->setEnabled(enabled);
  for (auto& row : motorRows_) {
    row.slider->setEnabled(enabled);
  }

  if (!enabled) {
    resetAllToIdle();
  }
}

void MotorControlWidget::onThreeDModeToggled(bool enabled) {
  if (enabled) {
    minThrottle_ = 0;
    maxThrottle_ = 2000;
    idleThrottle_ = 1000;
  } else {
    minThrottle_ = 1000;
    maxThrottle_ = 2000;
    idleThrottle_ = 1000;
  }

  applyRange();
  resetAllToIdle();
}

void MotorControlWidget::onMasterSliderChanged(int value) {
  for (auto& row : motorRows_) {
    QSignalBlocker blocker(row.slider);
    row.slider->setValue(value);
  }

  updateLabels();
}

void MotorControlWidget::onIndividualSliderChanged(int /*index*/, int /*value*/) {
  syncMasterFromIndividuals();
  updateLabels();
}

void MotorControlWidget::applyRange() {
  masterSlider_->setRange(minThrottle_, maxThrottle_);
  for (auto& row : motorRows_) {
    row.slider->setRange(minThrottle_, maxThrottle_);
  }

  rangeLabel_->setText(
      tr("Throttle range: %1..%2 (idle/neutral: %3)").arg(minThrottle_).arg(maxThrottle_).arg(
          idleThrottle_));
}

void MotorControlWidget::resetAllToIdle() {
  {
    QSignalBlocker masterBlocker(masterSlider_);
    masterSlider_->setValue(idleThrottle_);
  }

  for (auto& row : motorRows_) {
    QSignalBlocker blocker(row.slider);
    row.slider->setValue(idleThrottle_);
  }

  updateLabels();
}

void MotorControlWidget::syncMasterFromIndividuals() {
  if (motorRows_.isEmpty()) {
    return;
  }

  const int first = motorRows_.first().slider->value();
  bool allEqual = true;
  for (const auto& row : motorRows_) {
    if (row.slider->value() != first) {
      allEqual = false;
      break;
    }
  }

  if (allEqual) {
    QSignalBlocker blocker(masterSlider_);
    masterSlider_->setValue(first);
  }
}

void MotorControlWidget::updateLabels() {
  masterValueLabel_->setText(QString::number(masterSlider_->value()));
  for (const auto& row : motorRows_) {
    row.value->setText(QString::number(row.slider->value()));
  }
}
