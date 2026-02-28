#pragma once

#include <QWidget>

class QLabel;
class QSpinBox;

class Number final : public QWidget {
  Q_OBJECT

 public:
  explicit Number(const QString& label, QWidget* parent = nullptr);

  void setRange(int min, int max);
  int value() const;
  void setValue(int value);

 signals:
  void valueChanged(int value);

 private:
  QLabel* label_;
  QSpinBox* spinBox_;
};
