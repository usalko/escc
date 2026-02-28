#include "StatusBarWidget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QtGlobal>

StatusBarWidget::StatusBarWidget(QWidget* parent) : QWidget(parent) {
  setObjectName(QStringLiteral("statusBarWidget"));

  auto* row = new QHBoxLayout(this);
  row->setContentsMargins(0, 0, 0, 0);
  row->setSpacing(12);

  fcVersionLabel_ = new QLabel(this);
  connectionLabel_ = new QLabel(this);
  utilizationLabel_ = new QLabel(this);
  packetErrorsLabel_ = new QLabel(this);

  row->addWidget(fcVersionLabel_);
  row->addWidget(connectionLabel_);
  row->addWidget(utilizationLabel_);
  row->addWidget(packetErrorsLabel_);
  row->addStretch(1);

  updateTexts();
}

void StatusBarWidget::setFcVersion(const QString& version) {
  fcVersion_ = version.trimmed().isEmpty() ? QStringLiteral("N/A") : version.trimmed();
  updateTexts();
}

void StatusBarWidget::setConnectionStatus(bool connected) {
  connected_ = connected;
  updateTexts();
}

void StatusBarWidget::setPortUtilization(int downPercent, int upPercent) {
  portDownPercent_ = qBound(0, downPercent, 100);
  portUpPercent_ = qBound(0, upPercent, 100);
  updateTexts();
}

void StatusBarWidget::setPacketErrors(int count) {
  packetErrors_ = qMax(0, count);
  updateTexts();
}

void StatusBarWidget::incrementPacketErrors(int amount) {
  if (amount <= 0) {
    return;
  }

  packetErrors_ += amount;
  updateTexts();
}

void StatusBarWidget::updateTexts() {
  fcVersionLabel_->setText(tr("FC: %1").arg(fcVersion_));
  connectionLabel_->setText(tr("Connection: %1").arg(connected_ ? tr("Connected") : tr("Disconnected")));
  utilizationLabel_->setText(tr("Port Utilization D: %1% U: %2%").arg(portDownPercent_).arg(portUpPercent_));
  packetErrorsLabel_->setText(tr("Packet Errors: %1").arg(packetErrors_));
}
