#include "SerialPort.hpp"

SerialPort::SerialPort(QObject* parent) : QObject(parent) {
#if ESCC_HAS_QT_SERIALPORT
  connect(&port_, &QSerialPort::readyRead, this, &SerialPort::onReadyRead);
  connect(&port_, &QSerialPort::errorOccurred, this,
          [this](QSerialPort::SerialPortError error) {
            if (error != QSerialPort::NoError) {
              emit errorOccurred(port_.errorString());
            }
          });
#endif
}

bool SerialPort::open(const QString& portName, qint32 baudRate) {
#if ESCC_HAS_QT_SERIALPORT
  if (port_.isOpen()) {
    port_.close();
  }

  port_.setPortName(portName);
  port_.setBaudRate(baudRate);
  port_.setDataBits(QSerialPort::Data8);
  port_.setParity(QSerialPort::NoParity);
  port_.setStopBits(QSerialPort::OneStop);
  port_.setFlowControl(QSerialPort::NoFlowControl);

  return port_.open(QIODevice::ReadWrite);
#else
  Q_UNUSED(portName);
  Q_UNUSED(baudRate);
  emit errorOccurred(tr("Qt SerialPort module is not available in this build."));
  return false;
#endif
}

void SerialPort::close() {
#if ESCC_HAS_QT_SERIALPORT
  if (port_.isOpen()) {
    port_.close();
  }
#endif
}

bool SerialPort::isOpen() const {
#if ESCC_HAS_QT_SERIALPORT
  return port_.isOpen();
#else
  return false;
#endif
}

qint64 SerialPort::bytesSent() const { return sent_; }

qint64 SerialPort::bytesReceived() const { return received_; }

void SerialPort::write(const QByteArray& payload) {
#if ESCC_HAS_QT_SERIALPORT
  if (!port_.isOpen()) {
    return;
  }

  const auto written = port_.write(payload);
  if (written > 0) {
    sent_ += written;
  }
#else
  Q_UNUSED(payload);
  emit errorOccurred(tr("Serial write attempted without Qt SerialPort support."));
#endif
}

void SerialPort::onReadyRead() {
#if ESCC_HAS_QT_SERIALPORT
  const auto bytes = port_.readAll();
  received_ += bytes.size();
  emit dataReceived(bytes);
#endif
}
