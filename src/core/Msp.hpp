#pragma once

#include <QByteArray>

namespace msp {

struct Frame {
  quint16 command{0};
  QByteArray payload;
  bool v2{false};
};

enum Command : quint16 {
  ApiVersion = 1,
  FcVariant = 2,
  FcVersion = 3,
  BoardInfo = 4,
  BuildInfo = 5,
  FeatureConfig = 36,
  Motor3DConfig = 124,
  BatteryState = 130,
  SetMotor = 214,
  SetPassthrough = 245,
  Ident = 100,
  Status = 101,
  Motor = 104,
  Set3D = 217,
  Uid = 160,
  SendDshotCommand = 0x3003,
};

QByteArray encodeV1(quint16 command, const QByteArray& payload);
QByteArray encodeV2(quint16 command, const QByteArray& payload, quint8 flags = 0);
QByteArray encode(quint16 command, const QByteArray& payload);

bool decode(const QByteArray& buffer, Frame* outFrame, int* consumedBytes = nullptr);

}  // namespace msp
