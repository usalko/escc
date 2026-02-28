#pragma once

#include <QWidget>

class QComboBox;
class QLabel;

class LabeledSelect final : public QWidget {
  Q_OBJECT

 public:
  explicit LabeledSelect(const QString& label, QWidget* parent = nullptr);

  void addOption(const QString& text, const QVariant& value);
  QVariant value() const;
  void setValue(const QVariant& value);

 signals:
  void valueChanged(const QVariant& value);

 private:
  QLabel* label_;
  QComboBox* comboBox_;
};
