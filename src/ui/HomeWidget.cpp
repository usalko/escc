#include "HomeWidget.hpp"

#include "PortPickerWidget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

HomeWidget::HomeWidget(QWidget* parent) : QWidget(parent) {
  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(20, 20, 20, 20);
  root->setSpacing(14);

  auto* title = new QLabel(tr("ESC Configurator"), this);
  title->setObjectName(QStringLiteral("homeTitle"));

  auto* subtitle = new QLabel(
      tr("Configure and flash BLHeli_S, Bluejay and AM32 ESC firmware."), this);
  subtitle->setWordWrap(true);

  auto* portPicker = new PortPickerWidget(this);

  auto* supportedHeader = new QLabel(tr("Supported firmware"), this);
  supportedHeader->setObjectName(QStringLiteral("homeSectionTitle"));

  auto* supportedText = new QLabel(tr("• BLHeli_S\n• Bluejay\n• AM32"), this);
  supportedText->setWordWrap(true);

  auto* quickStartHeader = new QLabel(tr("Quick start"), this);
  quickStartHeader->setObjectName(QStringLiteral("homeSectionTitle"));

  auto* actions = new QHBoxLayout();
  actions->setSpacing(10);

  auto* connectButton = new QPushButton(tr("Connect"), this);
  auto* readButton = new QPushButton(tr("Read Settings"), this);
  auto* flashButton = new QPushButton(tr("Flash Firmware"), this);

  actions->addWidget(connectButton);
  actions->addWidget(readButton);
  actions->addWidget(flashButton);
  actions->addStretch(1);

  root->addWidget(title);
  root->addWidget(subtitle);
  root->addWidget(portPicker);
  root->addSpacing(6);
  root->addWidget(supportedHeader);
  root->addWidget(supportedText);
  root->addSpacing(6);
  root->addWidget(quickStartHeader);
  root->addLayout(actions);
  root->addStretch(1);

  connect(connectButton, &QPushButton::clicked, this, &HomeWidget::connectRequested);
  connect(portPicker, &PortPickerWidget::connectRequested, this, [this](const QString&) {
    emit connectRequested();
  });
  connect(portPicker, &PortPickerWidget::disconnectRequested, this, &HomeWidget::disconnectRequested);
  connect(readButton, &QPushButton::clicked, this, &HomeWidget::readSettingsRequested);
  connect(flashButton, &QPushButton::clicked, this, &HomeWidget::flashFirmwareRequested);
}
