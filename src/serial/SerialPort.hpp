#pragma once

#include <QObject>

#if ESCC_HAS_QT_SERIALPORT
#include <QSerialPort>
#endif

class SerialPort final : public QObject {
  Q_OBJECT

 public:
  explicit SerialPort(QObject* parent = nullptr);

  bool open(const QString& portName, qint32 baudRate = QSerialPort::Baud115200);
  void close();
  bool isOpen() const;

  qint64 bytesSent() const;
  qint64 bytesReceived() const;

  void write(const QByteArray& payload);

 signals:
  void dataReceived(const QByteArray& data);
  void errorOccurred(const QString& message);

 private slots:
  void onReadyRead();

 private:
#if ESCC_HAS_QT_SERIALPORT
  QSerialPort port_;
#endif
  qint64 sent_{0};
  qint64 received_{0};
};
