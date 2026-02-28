#include "FourWayHelper.hpp"

namespace fourway_helper {

std::optional<QString> commandToString(fourway::Command command) {
  switch (command) {
    case fourway::Command::InterfaceTestAlive:
      return QStringLiteral("InterfaceTestAlive");
    case fourway::Command::ProtocolGetVersion:
      return QStringLiteral("ProtocolGetVersion");
    case fourway::Command::InterfaceGetName:
      return QStringLiteral("InterfaceGetName");
    case fourway::Command::InterfaceGetVersion:
      return QStringLiteral("InterfaceGetVersion");
    case fourway::Command::InterfaceExit:
      return QStringLiteral("InterfaceExit");
    case fourway::Command::DeviceReset:
      return QStringLiteral("DeviceReset");
    case fourway::Command::DeviceInitFlash:
      return QStringLiteral("DeviceInitFlash");
    case fourway::Command::DeviceEraseAll:
      return QStringLiteral("DeviceEraseAll");
    case fourway::Command::DevicePageErase:
      return QStringLiteral("DevicePageErase");
    case fourway::Command::DeviceRead:
      return QStringLiteral("DeviceRead");
    case fourway::Command::DeviceWrite:
      return QStringLiteral("DeviceWrite");
    case fourway::Command::DeviceC2CKLow:
      return QStringLiteral("DeviceC2CKLow");
    case fourway::Command::DeviceReadEEprom:
      return QStringLiteral("DeviceReadEEprom");
    case fourway::Command::DeviceWriteEEprom:
      return QStringLiteral("DeviceWriteEEprom");
    case fourway::Command::InterfaceSetMode:
      return QStringLiteral("InterfaceSetMode");
  }

  return std::nullopt;
}

std::optional<QString> ackToString(fourway::Ack ack) {
  switch (ack) {
    case fourway::Ack::Ok:
      return QStringLiteral("Ok");
    case fourway::Ack::InterfaceUnknownError:
      return QStringLiteral("InterfaceUnknownError");
    case fourway::Ack::InterfaceInvalidCommand:
      return QStringLiteral("InterfaceInvalidCommand");
    case fourway::Ack::InterfaceInvalidCrc:
      return QStringLiteral("InterfaceInvalidCrc");
    case fourway::Ack::InterfaceVerifyError:
      return QStringLiteral("InterfaceVerifyError");
    case fourway::Ack::DeviceInvalidCommand:
      return QStringLiteral("DeviceInvalidCommand");
    case fourway::Ack::DeviceCommandFailed:
      return QStringLiteral("DeviceCommandFailed");
    case fourway::Ack::DeviceUnknownError:
      return QStringLiteral("DeviceUnknownError");
    case fourway::Ack::InterfaceInvalidChannel:
      return QStringLiteral("InterfaceInvalidChannel");
    case fourway::Ack::InterfaceInvalidParam:
      return QStringLiteral("InterfaceInvalidParam");
    case fourway::Ack::DeviceGeneralError:
      return QStringLiteral("DeviceGeneralError");
  }

  return std::nullopt;
}

}  // namespace fourway_helper
