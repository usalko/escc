#include "ButtonBarWidget.hpp"

#include <QHBoxLayout>
#include <QPushButton>

ButtonBarWidget::ButtonBarWidget(QWidget* parent) : QWidget(parent) {
  setObjectName(QStringLiteral("buttonBar"));

  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(12, 10, 12, 10);
  layout->setSpacing(8);

  auto* readSettingsButton = new QPushButton(tr("Read Settings"), this);
  auto* writeSettingsButton = new QPushButton(tr("Write Settings"), this);
  auto* flashAllButton = new QPushButton(tr("Flash All"), this);
  auto* selectFirmwareButton = new QPushButton(tr("Select Firmware"), this);

  layout->addWidget(readSettingsButton);
  layout->addWidget(writeSettingsButton);
  layout->addWidget(flashAllButton);
  layout->addWidget(selectFirmwareButton);
  layout->addStretch(1);

  connect(readSettingsButton,
          &QPushButton::clicked,
          this,
          &ButtonBarWidget::readSettingsRequested);
  connect(writeSettingsButton,
          &QPushButton::clicked,
          this,
          &ButtonBarWidget::writeSettingsRequested);
  connect(flashAllButton, &QPushButton::clicked, this, &ButtonBarWidget::flashAllRequested);
  connect(selectFirmwareButton,
          &QPushButton::clicked,
          this,
          &ButtonBarWidget::selectFirmwareRequested);
}
