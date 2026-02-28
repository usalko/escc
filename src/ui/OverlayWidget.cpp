#include "OverlayWidget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>

OverlayWidget::OverlayWidget(QWidget* parent) : QWidget(parent) {
  setObjectName(QStringLiteral("overlayWidget"));
  setAttribute(Qt::WA_StyledBackground, true);

  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(0, 0, 0, 0);
  root->setAlignment(Qt::AlignCenter);

  panel_ = new QWidget(this);
  panel_->setObjectName(QStringLiteral("overlayPanel"));
  panel_->setMinimumWidth(420);

  auto* panelLayout = new QVBoxLayout(panel_);
  panelLayout->setContentsMargins(12, 12, 12, 12);
  panelLayout->setSpacing(10);

  auto* topRow = new QHBoxLayout();
  topRow->setContentsMargins(0, 0, 0, 0);
  topRow->setSpacing(8);

  headlineLabel_ = new QLabel(tr("Please wait"), panel_);
  headlineLabel_->setObjectName(QStringLiteral("homeSectionTitle"));

  closeButton_ = new QPushButton(tr("Close"), panel_);
  closeButton_->setObjectName(QStringLiteral("overlayCloseButton"));
  connect(closeButton_, &QPushButton::clicked, this, [this]() {
    hideOverlay();
    emit dismissed();
  });

  topRow->addWidget(headlineLabel_);
  topRow->addStretch(1);
  topRow->addWidget(closeButton_);

  messageLabel_ = new QLabel(panel_);
  messageLabel_->setWordWrap(true);

  panelLayout->addLayout(topRow);
  panelLayout->addWidget(messageLabel_);

  root->addWidget(panel_);

  hide();
}

void OverlayWidget::setHeadline(const QString& text) {
  headlineLabel_->setText(text.trimmed().isEmpty() ? tr("Please wait") : text);
}

void OverlayWidget::setMessage(const QString& text) {
  messageLabel_->setText(text);
  messageLabel_->setVisible(!text.trimmed().isEmpty());
}

void OverlayWidget::showOverlay(const QString& headline, const QString& message) {
  setHeadline(headline);
  setMessage(message);
  show();
  raise();
}

void OverlayWidget::hideOverlay() { hide(); }

void OverlayWidget::mousePressEvent(QMouseEvent* event) {
  if (!panel_->geometry().contains(event->position().toPoint())) {
    hideOverlay();
    emit dismissed();
  }

  QWidget::mousePressEvent(event);
}
