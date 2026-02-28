#pragma once

#include <array>
#include <functional>
#include <optional>

#include <QObject>
#include <QString>

namespace fourway {

constexpr quint8 kHostStartByte = 0x2F;
constexpr quint8 kDeviceStartByte = 0x2E;

enum class Command : quint8 {
  InterfaceTestAlive = 0x30,
  ProtocolGetVersion = 0x31,
  InterfaceGetName = 0x32,
  InterfaceGetVersion = 0x33,
  InterfaceExit = 0x34,
  DeviceReset = 0x35,
  DeviceInitFlash = 0x37,
  DeviceEraseAll = 0x38,
  DevicePageErase = 0x39,
  DeviceRead = 0x3A,
  DeviceWrite = 0x3B,
  DeviceC2CKLow = 0x3C,
  DeviceReadEEprom = 0x3D,
  DeviceWriteEEprom = 0x3E,
  InterfaceSetMode = 0x3F,
};

enum class Ack : quint8 {
  Ok = 0x00,
  InterfaceUnknownError = 0x01,
  InterfaceInvalidCommand = 0x02,
  InterfaceInvalidCrc = 0x03,
  InterfaceVerifyError = 0x04,
  DeviceInvalidCommand = 0x05,
  DeviceCommandFailed = 0x06,
  DeviceUnknownError = 0x07,
  InterfaceInvalidChannel = 0x08,
  InterfaceInvalidParam = 0x09,
  DeviceGeneralError = 0x0F,
};

enum class Mode : quint8 {
  SiLC2 = 0,
  SiLBLB = 1,
  AtmBLB = 2,
  AtmSK = 3,
  ARMBLB = 4,
};

enum class ParseStatus : quint8 {
  Ok = 0,
  InvalidStart = 1,
  NotEnoughData = 2,
  InvalidChecksum = 3,
};

struct Response {
  quint8 command{0};
  quint16 address{0};
  quint8 ack{0};
  QByteArray params;
  quint16 checksum{0};
};

struct InitFlashInfo {
  quint8 target{0};
  quint16 signature{0};
  quint8 interfaceMode{0};
  QByteArray rawParams;
};

constexpr std::array<Mode, 2> kSilabsModes{Mode::SiLC2, Mode::SiLBLB};
constexpr std::array<Mode, 2> kAtmelModes{Mode::AtmBLB, Mode::AtmSK};

constexpr bool isSilabsMode(Mode mode) {
  return mode == Mode::SiLC2 || mode == Mode::SiLBLB;
}

constexpr bool isAtmelMode(Mode mode) {
  return mode == Mode::AtmBLB || mode == Mode::AtmSK;
}

quint16 crc16XmodemUpdate(quint16 crc, quint8 byte);
quint16 crc16Xmodem(const QByteArray& bytes);

QByteArray createMessage(Command command, QByteArray params = {}, quint16 address = 0);
ParseStatus parseMessage(const QByteArray& bytes, Response* outResponse, int* consumedBytes = nullptr);

}  // namespace fourway

class FourWay final : public QObject {
  Q_OBJECT

 public:
  using Transport = std::function<QByteArray(const QByteArray& request)>;

  explicit FourWay(QObject* parent = nullptr);

  void setTransport(Transport transport);

  static QByteArray buildInterfaceTestAlive(quint16 address = 0);
  static QByteArray buildProtocolGetVersion(quint16 address = 0);
  static QByteArray buildInterfaceGetVersion(quint16 address = 0);
  static QByteArray buildInterfaceExit(quint16 address = 0);
  static QByteArray buildSetMode(fourway::Mode mode, quint16 address = 0);

  static bool isAckOk(const fourway::Response& response);
  static bool responseMatchesCommand(const fourway::Response& response,
                                     fourway::Command command);
  static bool validateOkResponse(const fourway::Response& response,
                                 fourway::Command expectedCommand,
                                 QString* error = nullptr);
  static std::optional<quint16> protocolVersionFromResponse(const fourway::Response& response);
  static std::optional<quint16> interfaceVersionFromResponse(const fourway::Response& response);

  std::optional<fourway::Response> exchange(fourway::Command command,
                                            const QByteArray& params = {},
                                            quint16 address = 0,
                                            bool expectResponse = true,
                                            QString* error = nullptr) const;
  bool testAlive(QString* error = nullptr) const;
  std::optional<quint16> getProtocolVersion(QString* error = nullptr) const;
  std::optional<fourway::Response> initFlash(quint8 target, QString* error = nullptr) const;
  std::optional<fourway::InitFlashInfo> initFlashInfo(quint8 target, QString* error = nullptr) const;
  bool reset(quint8 target, QString* error = nullptr) const;
  std::optional<QByteArray> read(quint16 address, int bytes, QString* error = nullptr) const;
  std::optional<QByteArray> readEeprom(quint16 address, int bytes, QString* error = nullptr) const;
  bool write(quint16 address, const QByteArray& data, QString* error = nullptr) const;
  bool writeEeprom(quint16 address, const QByteArray& data, QString* error = nullptr) const;
  std::optional<QByteArray> readSettings(quint8 target,
                                         quint16 eepromAddress,
                                         int length,
                                         QString* error = nullptr) const;
  bool writeSettings(quint8 target,
                     quint16 eepromAddress,
                     const QByteArray& settings,
                     QString* error = nullptr) const;
  std::optional<QByteArray> readSettingsAuto(quint8 target,
                                             int length,
                                             QString* error = nullptr) const;
  bool writeSettingsAuto(quint8 target,
                         const QByteArray& settings,
                         QString* error = nullptr) const;
  bool flashImage(quint8 target,
                  const QByteArray& image,
                  int startPage,
                  int stopPage,
                  QString* error = nullptr) const;
  bool flashImageModeAware(quint8 target,
                           const QByteArray& image,
                           QString* error = nullptr) const;

 private:
  Transport transport_;

 signals:
  void progressChanged(int percent);
};
