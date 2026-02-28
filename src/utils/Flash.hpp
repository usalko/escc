#pragma once

#include <optional>

#include <QByteArray>
#include <QString>
#include <QVector>

namespace flash {

struct HexBlock {
  quint32 address{0};
  QByteArray data;
};

struct ParsedHex {
  QVector<HexBlock> data;
  bool endOfFile{false};
  int bytes{0};
  quint32 startLinearAddress{0};
};

struct FlashInfo {
  struct Meta {
    quint16 signature{0};
    quint8 input{0};
    quint8 interfaceMode{0};
    bool available{true};
    QString am32FileName;
    QString am32McuType;
  } meta;

  QString displayName{QStringLiteral("UNKNOWN")};
  QString firmwareName{QStringLiteral("UNKNOWN")};
  bool supported{true};
  bool isAtmel{false};
  bool isSiLabs{false};
  bool isArm{false};
};

std::optional<QByteArray> fillImage(const ParsedHex& parsed, int size, quint32 flashOffset);
std::optional<ParsedHex> parseHex(const QString& content, QString* error = nullptr);
std::optional<FlashInfo> getInfo(const QByteArray& params, QString* error = nullptr);

}  // namespace flash
