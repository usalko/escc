#pragma once

#include <QWidget>

class FlashWidget final : public QWidget {
  Q_OBJECT

 public:
  explicit FlashWidget(QWidget* parent = nullptr);
};
