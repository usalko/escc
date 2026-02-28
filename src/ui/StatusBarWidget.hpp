#pragma once

#include <QWidget>

class QLabel;

class StatusBarWidget final : public QWidget {
  Q_OBJECT

 public:
  explicit StatusBarWidget(QWidget* parent = nullptr);

  void setFcVersion(const QString& version);
  void setConnectionStatus(bool connected);
  void setPortUtilization(int downPercent, int upPercent);
  void setPacketErrors(int count);
  void incrementPacketErrors(int amount = 1);

 private:
  void updateTexts();

  QString fcVersion_{QStringLiteral("N/A")};
  bool connected_{false};
  int portDownPercent_{0};
  int portUpPercent_{0};
  int packetErrors_{0};

  QLabel* fcVersionLabel_{nullptr};
  QLabel* connectionLabel_{nullptr};
  QLabel* utilizationLabel_{nullptr};
  QLabel* packetErrorsLabel_{nullptr};
};
