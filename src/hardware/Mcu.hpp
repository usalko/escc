#pragma once

#include <optional>

#include <QString>

namespace hardware {

struct McuProfile {
  QString name;
  quint16 signature{0};
  quint8 interfaceMode{0};
  int pageSize{0};
  int flashSize{0};
  std::optional<quint32> flashOffset;
  std::optional<quint32> firmwareStart;
  std::optional<quint32> eepromOffset;
  std::optional<quint32> bootloaderAddress;
  std::optional<quint32> lockByteAddress;
};

std::optional<McuProfile> findMcuProfile(quint8 interfaceMode, quint16 signature);

}  // namespace hardware
