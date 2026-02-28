#include "CommonSettingsWidget.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

CommonSettingsWidget::CommonSettingsWidget(QWidget* parent)
    : QWidget(parent), startupPowerValueLabel_(new QLabel(this)) {
  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(10);

  auto* generalGroup = new QGroupBox(tr("General"), this);
  auto* generalForm = new QFormLayout(generalGroup);

  auto* pwmFrequency = new QComboBox(generalGroup);
  pwmFrequency->addItem(QStringLiteral("24 kHz"), 24);
  pwmFrequency->addItem(QStringLiteral("48 kHz"), 48);
  pwmFrequency->addItem(QStringLiteral("96 kHz"), 96);
  pwmFrequency->setCurrentIndex(1);

  auto* motorDirection = new QComboBox(generalGroup);
  motorDirection->addItem(tr("Normal"), 0);
  motorDirection->addItem(tr("Reversed"), 1);

  generalForm->addRow(tr("PWM Frequency"), pwmFrequency);
  generalForm->addRow(tr("Motor Direction"), motorDirection);

  auto* startupGroup = new QGroupBox(tr("Startup"), this);
  auto* startupLayout = new QVBoxLayout(startupGroup);

  auto* startupRow = new QHBoxLayout();
  auto* startupPower = new QSlider(Qt::Horizontal, startupGroup);
  startupPower->setRange(1, 15);
  startupPower->setValue(8);

  startupPowerValueLabel_->setText(sliderValueText(startupPower->value()));
  startupPowerValueLabel_->setMinimumWidth(32);

  startupRow->addWidget(startupPower, 1);
  startupRow->addWidget(startupPowerValueLabel_);

  startupLayout->addWidget(new QLabel(tr("Startup Power"), startupGroup));
  startupLayout->addLayout(startupRow);

  auto* safetyGroup = new QGroupBox(tr("Safety"), this);
  auto* safetyLayout = new QVBoxLayout(safetyGroup);

  auto* beaconEnabled = new QCheckBox(tr("Beacon enabled"), safetyGroup);
  beaconEnabled->setChecked(true);

  auto* temperatureProtection = new QCheckBox(tr("Temperature protection"), safetyGroup);
  temperatureProtection->setChecked(true);

  safetyLayout->addWidget(beaconEnabled);
  safetyLayout->addWidget(temperatureProtection);

  root->addWidget(generalGroup);
  root->addWidget(startupGroup);
  root->addWidget(safetyGroup);
  root->addStretch(1);

  connect(startupPower, &QSlider::valueChanged, this, [this](int value) {
    startupPowerValueLabel_->setText(sliderValueText(value));
  });
}

QString CommonSettingsWidget::sliderValueText(int value) {
  return QString::number(value);
}
