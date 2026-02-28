#include "FlashWidget.hpp"

#include "CommonSettingsWidget.hpp"
#include "EscWidget.hpp"

#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

FlashWidget::FlashWidget(QWidget* parent) : QWidget(parent) {
  setObjectName(QStringLiteral("flashWidget"));

  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(16, 16, 16, 16);
  root->setSpacing(10);

  auto* title = new QLabel(tr("Flash"), this);
  title->setObjectName(QStringLiteral("homeTitle"));

  auto* subtitle = new QLabel(tr("Main ESC settings and firmware operations."), this);
  subtitle->setWordWrap(true);

  auto* wrapper = new QWidget(this);
  auto* columns = new QHBoxLayout(wrapper);
  columns->setContentsMargins(0, 0, 0, 0);
  columns->setSpacing(12);

  auto* commonBox = new QGroupBox(tr("Common Settings"), wrapper);
  commonBox->setObjectName(QStringLiteral("flashCommonBox"));
  auto* commonLayout = new QVBoxLayout(commonBox);
  commonLayout->addWidget(new CommonSettingsWidget(commonBox));

  auto* individualBox = new QGroupBox(tr("Individual ESCs"), wrapper);
  individualBox->setObjectName(QStringLiteral("flashIndividualBox"));
  auto* individualLayout = new QVBoxLayout(individualBox);
  auto* cardsContainer = new QWidget(individualBox);
  auto* cardsLayout = new QVBoxLayout(cardsContainer);
  cardsLayout->setContentsMargins(0, 0, 0, 0);
  cardsLayout->setSpacing(10);

  for (int i = 0; i < 3; ++i) {
    auto* escWidget = new EscWidget(i, cardsContainer);
    escWidget->setDisplayName(tr("Not loaded"));
    cardsLayout->addWidget(escWidget);
  }
  cardsLayout->addStretch(1);

  auto* scroll = new QScrollArea(individualBox);
  scroll->setWidgetResizable(true);
  scroll->setFrameShape(QFrame::NoFrame);
  scroll->setWidget(cardsContainer);
  individualLayout->addWidget(scroll, 1);

  columns->addWidget(commonBox, 1);
  columns->addWidget(individualBox, 1);

  root->addWidget(title);
  root->addWidget(subtitle);
  root->addWidget(wrapper, 1);
}
