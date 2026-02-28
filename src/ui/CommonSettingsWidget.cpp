#include "CommonSettingsWidget.hpp"

#include "Input/Checkbox.hpp"
#include "Input/Info.hpp"
#include "Input/LabeledSelect.hpp"
#include "Input/Number.hpp"
#include "Input/Slider.hpp"

#include <QGroupBox>
#include <QVBoxLayout>

CommonSettingsWidget::CommonSettingsWidget(QWidget* parent)
    : QWidget(parent) {
  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(10);

  auto* info = new Info(
      tr("These common settings apply to all connected ESCs. Individual overrides can be set per ESC card."),
      this);

  auto* generalGroup = new QGroupBox(tr("General"), this);
  auto* generalLayout = new QVBoxLayout(generalGroup);
  generalLayout->setSpacing(8);

  auto* pwmFrequency = new LabeledSelect(tr("PWM Frequency"), generalGroup);
  pwmFrequency->addOption(QStringLiteral("24 kHz"), 24);
  pwmFrequency->addOption(QStringLiteral("48 kHz"), 48);
  pwmFrequency->addOption(QStringLiteral("96 kHz"), 96);
  pwmFrequency->setValue(48);

  auto* motorDirection = new LabeledSelect(tr("Motor Direction"), generalGroup);
  motorDirection->addOption(tr("Normal"), 0);
  motorDirection->addOption(tr("Reversed"), 1);
  motorDirection->setValue(0);

  generalLayout->addWidget(pwmFrequency);
  generalLayout->addWidget(motorDirection);

  auto* startupGroup = new QGroupBox(tr("Startup"), this);
  auto* startupLayout = new QVBoxLayout(startupGroup);

  auto* startupPower = new Slider(tr("Startup Power"), startupGroup);
  startupPower->setRange(1, 15);
  startupPower->setValue(8);

  auto* beaconDelay = new Number(tr("Beacon Delay (s)"), startupGroup);
  beaconDelay->setRange(1, 120);
  beaconDelay->setValue(10);

  startupLayout->addWidget(startupPower);
  startupLayout->addWidget(beaconDelay);

  auto* safetyGroup = new QGroupBox(tr("Safety"), this);
  auto* safetyLayout = new QVBoxLayout(safetyGroup);

  auto* beaconEnabled = new Checkbox(tr("Beacon enabled"), safetyGroup);
  beaconEnabled->setChecked(true);

  auto* temperatureProtection = new Checkbox(tr("Temperature protection"), safetyGroup);
  temperatureProtection->setChecked(true);

  root->addWidget(info);
  safetyLayout->addWidget(beaconEnabled);
  safetyLayout->addWidget(temperatureProtection);

  root->addWidget(generalGroup);
  root->addWidget(startupGroup);
  root->addWidget(safetyGroup);
  root->addStretch(1);
}
