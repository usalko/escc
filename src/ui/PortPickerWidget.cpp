#include "PortPickerWidget.hpp"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#if ESCC_HAS_QT_SERIALPORT
#include <QSerialPortInfo>
#endif

PortPickerWidget::PortPickerWidget(QWidget* parent)
    : QWidget(parent),
      portsCombo_(new QComboBox(this)),
      compatibilityLabel_(new QLabel(this)),
      connectButton_(new QPushButton(tr("Connect"), this)),
      disconnectButton_(new QPushButton(tr("Disconnect"), this)) {
  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(8);

  auto* title = new QLabel(tr("Port"), this);
  title->setObjectName(QStringLiteral("homeSectionTitle"));

  auto* row = new QHBoxLayout();
  row->setSpacing(8);
  portsCombo_->setMinimumWidth(240);

  auto* refreshButton = new QPushButton(tr("Refresh"), this);

  row->addWidget(portsCombo_, 1);
  row->addWidget(connectButton_);
  row->addWidget(disconnectButton_);
  row->addWidget(refreshButton);

  root->addWidget(title);
  root->addLayout(row);
  root->addWidget(compatibilityLabel_);

  disconnectButton_->setEnabled(false);

  connect(refreshButton, &QPushButton::clicked, this, &PortPickerWidget::refreshPorts);
  connect(portsCombo_, &QComboBox::currentIndexChanged, this, &PortPickerWidget::onSelectionChanged);
  connect(connectButton_, &QPushButton::clicked, this, &PortPickerWidget::onConnectClicked);
  connect(disconnectButton_, &QPushButton::clicked, this, &PortPickerWidget::onDisconnectClicked);

  refreshPorts();
}

void PortPickerWidget::refreshPorts() {
  portsCombo_->clear();

#if ESCC_HAS_QT_SERIALPORT
  const auto ports = QSerialPortInfo::availablePorts();
  for (const auto& info : ports) {
    const QString display = QStringLiteral("%1 (%2)").arg(info.portName(), info.description());
    portsCombo_->addItem(display, info.portName());
  }

  if (portsCombo_->count() == 0) {
    portsCombo_->addItem(tr("No serial ports found"), QString());
    connectButton_->setEnabled(false);
  } else {
    connectButton_->setEnabled(!connected_);
  }
#else
  portsCombo_->addItem(tr("SerialPort module unavailable"), QString());
  connectButton_->setEnabled(false);
  compatibilityLabel_->setText(tr("Compatibility: unavailable (Qt SerialPort not built)"));
#endif

  onSelectionChanged();
}

void PortPickerWidget::onSelectionChanged() {
  const QString portName = portsCombo_->currentData().toString();
  compatibilityLabel_->setText(tr("Compatibility: %1").arg(detectCompatibility(portName)));
}

void PortPickerWidget::onConnectClicked() {
  const QString portName = portsCombo_->currentData().toString();
  if (portName.isEmpty()) {
    return;
  }

  connected_ = true;
  connectButton_->setEnabled(false);
  disconnectButton_->setEnabled(true);
  emit connectRequested(portName);
}

void PortPickerWidget::onDisconnectClicked() {
  connected_ = false;
  connectButton_->setEnabled(portsCombo_->count() > 0 && !portsCombo_->currentData().toString().isEmpty());
  disconnectButton_->setEnabled(false);
  emit disconnectRequested();
}

QString PortPickerWidget::detectCompatibility(const QString& portName) const {
  if (portName.isEmpty()) {
    return tr("no port selected");
  }

  const QString normalized = portName.toLower();
  if (normalized.contains(QStringLiteral("ttyusb")) ||
      normalized.contains(QStringLiteral("ttyacm")) ||
      normalized.contains(QStringLiteral("com"))) {
    return tr("likely compatible");
  }

  return tr("unknown");
}
