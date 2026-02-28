#include "FlashWidget.hpp"

#include "CommonSettingsWidget.hpp"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
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
  individualLayout->addWidget(new QLabel(tr("Per-ESC cards will be added in 4.5.3."), individualBox));

  auto* escList = new QListWidget(individualBox);
  escList->addItem(tr("ESC #1 — not loaded"));
  escList->addItem(tr("ESC #2 — not loaded"));
  escList->addItem(tr("ESC #3 — not loaded"));
  individualLayout->addWidget(escList, 1);

  columns->addWidget(commonBox, 1);
  columns->addWidget(individualBox, 1);

  root->addWidget(title);
  root->addWidget(subtitle);
  root->addWidget(wrapper, 1);
}
