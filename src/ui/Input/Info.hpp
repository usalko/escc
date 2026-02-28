#pragma once

#include <QWidget>

class QLabel;

class Info final : public QWidget {
  Q_OBJECT

 public:
  explicit Info(const QString& text, QWidget* parent = nullptr);

  void setText(const QString& text);

 private:
  QLabel* label_;
};
