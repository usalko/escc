#pragma once

#include <QObject>

#include "serial/QueueProcessor.hpp"
#include "serial/SerialPort.hpp"

class Serial final : public QObject {
  Q_OBJECT

 public:
  explicit Serial(QObject* parent = nullptr);

  SerialPort* port();
  QueueProcessor* queue();

 private:
  SerialPort serialPort_;
  QueueProcessor queueProcessor_;
};
