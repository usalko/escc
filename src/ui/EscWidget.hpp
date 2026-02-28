#pragma once

#include <QWidget>

class QLabel;
class QProgressBar;
class QPushButton;

class EscWidget final : public QWidget {
  Q_OBJECT

 public:
  explicit EscWidget(int index, QWidget* parent = nullptr);

  void setDisplayName(const QString& displayName);
  void setProgress(int percent);

 signals:
  void flashRequested(int index);
  void firmwareDumpRequested(int index);

 private:
  void updateTitle();

  int index_;
  QString displayName_;
  QLabel* titleLabel_;
  QProgressBar* progressBar_;
  QPushButton* flashButton_;
  QPushButton* firmwareDumpButton_;
};
