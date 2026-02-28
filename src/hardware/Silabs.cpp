#include "Silabs.hpp"

namespace hardware {

bool SilabsHardware::erasePage(const FourWay& fourWay,
                               int page,
                               const McuProfile& profile,
                               QString* error) const {
  if (page < 0 || page > 255) {
    if (error != nullptr) {
      *error = QObject::tr("Page index must be in range 0..255");
    }
    return false;
  }

  int pageMultiplier = 1;
  if (profile.pageSize > 512) {
    pageMultiplier = 4;
  }

  const int encodedPage = page * pageMultiplier;
  if (encodedPage > 255) {
    if (error != nullptr) {
      *error = QObject::tr("Encoded page index exceeds protocol limit");
    }
    return false;
  }

  QByteArray params;
  params.append(static_cast<char>(encodedPage));
  const auto response = fourWay.exchange(fourway::Command::DevicePageErase, params, 0, true, error);
  return response.has_value();
}

std::optional<QByteArray> SilabsHardware::readChunk(const FourWay& fourWay,
                                                    quint16 address,
                                                    int length,
                                                    QString* error) const {
  return fourWay.read(address, length, error);
}

bool SilabsHardware::writeChunk(const FourWay& fourWay,
                                quint16 address,
                                const QByteArray& data,
                                QString* error) const {
  return fourWay.write(address, data, error);
}

}  // namespace hardware
