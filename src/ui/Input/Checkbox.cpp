#include "Checkbox.hpp"

#include <QCheckBox>
#include <QHBoxLayout>

Checkbox::Checkbox(const QString& label, QWidget* parent)
    : QWidget(parent), checkbox_(new QCheckBox(label, this)) {
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(checkbox_);
  layout->addStretch(1);

  connect(checkbox_, &QCheckBox::toggled, this, &Checkbox::toggled);
}

bool Checkbox::checked() const {
  return checkbox_->isChecked();
}

void Checkbox::setChecked(bool value) {
  checkbox_->setChecked(value);
}
