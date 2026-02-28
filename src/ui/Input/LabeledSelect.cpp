#include "LabeledSelect.hpp"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>

LabeledSelect::LabeledSelect(const QString& label, QWidget* parent)
    : QWidget(parent), label_(new QLabel(label, this)), comboBox_(new QComboBox(this)) {
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);
  layout->addWidget(label_);
  layout->addWidget(comboBox_, 1);

  connect(comboBox_, &QComboBox::currentIndexChanged, this, [this](int) {
    emit valueChanged(comboBox_->currentData());
  });
}

void LabeledSelect::addOption(const QString& text, const QVariant& value) {
  comboBox_->addItem(text, value);
}

QVariant LabeledSelect::value() const {
  return comboBox_->currentData();
}

void LabeledSelect::setValue(const QVariant& value) {
  const int index = comboBox_->findData(value);
  if (index >= 0) {
    comboBox_->setCurrentIndex(index);
  }
}
