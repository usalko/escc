#include "Number.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

Number::Number(const QString& label, QWidget* parent)
    : QWidget(parent), label_(new QLabel(label, this)), spinBox_(new QSpinBox(this)) {
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);
  layout->addWidget(label_);
  layout->addWidget(spinBox_);
  layout->addStretch(1);

  connect(spinBox_, &QSpinBox::valueChanged, this, &Number::valueChanged);
}

void Number::setRange(int min, int max) {
  spinBox_->setRange(min, max);
}

int Number::value() const {
  return spinBox_->value();
}

void Number::setValue(int value) {
  spinBox_->setValue(value);
}
