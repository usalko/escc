#include "Arm.hpp"

namespace hardware {

bool ArmHardware::erasePage(const FourWay& fourWay,
                            int page,
                            const McuProfile& profile,
                            QString* error) const {
  Q_UNUSED(profile);

  if (page < 0 || page > 255) {
    if (error != nullptr) {
      *error = QObject::tr("Page index must be in range 0..255");
    }
    return false;
  }

  QByteArray params;
  params.append(static_cast<char>(page));
  const auto response = fourWay.exchange(fourway::Command::DevicePageErase, params, 0, true, error);
  return response.has_value();
}

std::optional<QByteArray> ArmHardware::readChunk(const FourWay& fourWay,
                                                 quint16 address,
                                                 int length,
                                                 QString* error) const {
  return fourWay.read(address, length, error);
}

bool ArmHardware::writeChunk(const FourWay& fourWay,
                             quint16 address,
                             const QByteArray& data,
                             QString* error) const {
  return fourWay.write(address, data, error);
}

}  // namespace hardware
