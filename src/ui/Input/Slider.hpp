#pragma once

#include <QWidget>

class QLabel;
class QSlider;

class Slider final : public QWidget {
  Q_OBJECT

 public:
  explicit Slider(const QString& label, QWidget* parent = nullptr);

  void setRange(int min, int max);
  int value() const;
  void setValue(int value);

 signals:
  void valueChanged(int value);

 private:
  QLabel* titleLabel_;
  QSlider* slider_;
  QLabel* valueLabel_;
};
