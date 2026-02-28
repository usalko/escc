#include "Slider.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

Slider::Slider(const QString& label, QWidget* parent)
    : QWidget(parent),
      titleLabel_(new QLabel(label, this)),
      slider_(new QSlider(Qt::Horizontal, this)),
      valueLabel_(new QLabel(this)) {
  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(4);

  auto* row = new QHBoxLayout();
  row->setContentsMargins(0, 0, 0, 0);
  row->setSpacing(8);
  row->addWidget(slider_, 1);
  row->addWidget(valueLabel_);

  root->addWidget(titleLabel_);
  root->addLayout(row);

  valueLabel_->setMinimumWidth(28);
  valueLabel_->setText(QString::number(slider_->value()));

  connect(slider_, &QSlider::valueChanged, this, [this](int value) {
    valueLabel_->setText(QString::number(value));
    emit valueChanged(value);
  });
}

void Slider::setRange(int min, int max) {
  slider_->setRange(min, max);
}

int Slider::value() const {
  return slider_->value();
}

void Slider::setValue(int value) {
  slider_->setValue(value);
}
