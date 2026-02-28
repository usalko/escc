#pragma once

#include <QWidget>

class QLabel;
class QPushButton;

class OverlayWidget final : public QWidget {
  Q_OBJECT

 public:
  explicit OverlayWidget(QWidget* parent = nullptr);

  void setHeadline(const QString& text);
  void setMessage(const QString& text);
  void showOverlay(const QString& headline, const QString& message = QString());
  void hideOverlay();

 signals:
  void dismissed();

 protected:
  void mousePressEvent(QMouseEvent* event) override;

 private:
  QWidget* panel_{nullptr};
  QLabel* headlineLabel_{nullptr};
  QLabel* messageLabel_{nullptr};
  QPushButton* closeButton_{nullptr};
};
