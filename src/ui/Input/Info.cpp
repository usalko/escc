#include "Info.hpp"

#include <QLabel>
#include <QVBoxLayout>

Info::Info(const QString& text, QWidget* parent)
    : QWidget(parent), label_(new QLabel(text, this)) {
  setObjectName(QStringLiteral("infoWidget"));

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(8, 8, 8, 8);

  label_->setWordWrap(true);
  layout->addWidget(label_);
}

void Info::setText(const QString& text) {
  label_->setText(text);
}
