#pragma once

#include <QWidget>

class QCheckBox;

class Checkbox final : public QWidget {
  Q_OBJECT

 public:
  explicit Checkbox(const QString& label, QWidget* parent = nullptr);

  bool checked() const;
  void setChecked(bool value);

 signals:
  void toggled(bool checked);

 private:
  QCheckBox* checkbox_;
};
