#include "EscWidget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

EscWidget::EscWidget(int index, QWidget* parent)
    : QWidget(parent),
      index_(index),
      displayName_(tr("Unsupported/Unrecognized")),
      titleLabel_(new QLabel(this)),
      progressBar_(new QProgressBar(this)),
      flashButton_(new QPushButton(tr("Flash"), this)),
      firmwareDumpButton_(new QPushButton(tr("Firmware Dump"), this)) {
  setObjectName(QStringLiteral("escWidget"));

  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(12, 12, 12, 12);
  root->setSpacing(8);

  titleLabel_->setObjectName(QStringLiteral("escWidgetTitle"));
  updateTitle();

  progressBar_->setRange(0, 100);
  progressBar_->setValue(0);
  progressBar_->setTextVisible(true);
  progressBar_->setFormat(tr("%p%"));

  auto* buttons = new QHBoxLayout();
  buttons->setSpacing(8);
  buttons->addWidget(flashButton_);
  buttons->addWidget(firmwareDumpButton_);
  buttons->addStretch(1);

  root->addWidget(titleLabel_);
  root->addWidget(progressBar_);
  root->addLayout(buttons);

  connect(flashButton_, &QPushButton::clicked, this, [this]() { emit flashRequested(index_); });
  connect(firmwareDumpButton_, &QPushButton::clicked, this, [this]() {
    emit firmwareDumpRequested(index_);
  });
}

void EscWidget::setDisplayName(const QString& displayName) {
  displayName_ = displayName.trimmed().isEmpty() ? tr("Unsupported/Unrecognized") : displayName;
  updateTitle();
}

void EscWidget::setProgress(int percent) {
  progressBar_->setValue(qBound(0, percent, 100));
}

void EscWidget::updateTitle() {
  titleLabel_->setText(tr("ESC %1: %2").arg(index_ + 1).arg(displayName_));
}
