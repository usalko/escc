#include "Serial.hpp"

Serial::Serial(QObject* parent)
    : QObject(parent), serialPort_(this), queueProcessor_(this) {}

SerialPort* Serial::port() { return &serialPort_; }

QueueProcessor* Serial::queue() { return &queueProcessor_; }
