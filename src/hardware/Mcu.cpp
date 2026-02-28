#include "Mcu.hpp"

#include <array>

#include "core/FourWay.hpp"

namespace hardware {
namespace {

McuProfile makeProfile(const QString& name,
                       quint16 signature,
                       quint8 interfaceMode,
                       int pageSize,
                       int flashSize,
                       std::optional<quint32> flashOffset,
                       std::optional<quint32> firmwareStart,
                       std::optional<quint32> eepromOffset,
                       std::optional<quint32> bootloaderAddress,
                       std::optional<quint32> lockByteAddress) {
  McuProfile profile;
  profile.name = name;
  profile.signature = signature;
  profile.interfaceMode = interfaceMode;
  profile.pageSize = pageSize;
  profile.flashSize = flashSize;
  profile.flashOffset = flashOffset;
  profile.firmwareStart = firmwareStart;
  profile.eepromOffset = eepromOffset;
  profile.bootloaderAddress = bootloaderAddress;
  profile.lockByteAddress = lockByteAddress;
  return profile;
}

const std::array<McuProfile, 5> kProfiles = {{
  makeProfile("EFM8BB10x", 0xE8B1, static_cast<quint8>(fourway::Mode::SiLBLB), 512, 8192, 0x00, 0x00,
                0x1A00, 0x1C00, 0x1FFF),
  makeProfile("EFM8BB21x", 0xE8B2, static_cast<quint8>(fourway::Mode::SiLBLB), 512, 8192, 0x00, 0x00,
                0x1A00, 0x1C00, 0xFBFF),
  makeProfile("EFM8BB51x", 0xE8B5, static_cast<quint8>(fourway::Mode::SiLBLB), 2048, 63485, 0x00, 0x00,
                0x3000, 0xF000, 0xF7FF),
  makeProfile("STM32F051", 0x1F06, static_cast<quint8>(fourway::Mode::ARMBLB), 1024, 65536, 0x08000000,
                0x1000, 0x7C00, std::nullopt, std::nullopt),
  makeProfile("ARM64K", 0x3506, static_cast<quint8>(fourway::Mode::ARMBLB), 1024, 65536, 0x08000000,
                0x1000, 0xF800, std::nullopt, std::nullopt),
}};

}  // namespace

std::optional<McuProfile> findMcuProfile(quint8 interfaceMode, quint16 signature) {
  for (const auto& profile : kProfiles) {
    if (profile.interfaceMode == interfaceMode && profile.signature == signature) {
      return profile;
    }
  }

  return std::nullopt;
}

}  // namespace hardware
