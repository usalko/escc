#pragma once

#include <QWidget>

class QComboBox;
class QLabel;
class QPushButton;

class PortPickerWidget final : public QWidget {
  Q_OBJECT

 public:
  explicit PortPickerWidget(QWidget* parent = nullptr);

 signals:
  void connectRequested(const QString& portName);
  void disconnectRequested();

 private slots:
  void refreshPorts();
  void onSelectionChanged();
  void onConnectClicked();
  void onDisconnectClicked();

 private:
  QString detectCompatibility(const QString& portName) const;

  QComboBox* portsCombo_;
  QLabel* compatibilityLabel_;
  QPushButton* connectButton_;
  QPushButton* disconnectButton_;
  bool connected_{false};
};
