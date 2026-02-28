#include "Hardware.hpp"

#include <QtGlobal>

#include "hardware/Arm.hpp"
#include "hardware/Silabs.hpp"

namespace hardware {

namespace {

std::optional<quint16> toAddress(int offset, QString* error) {
  if (offset < 0 || offset > 0xFFFF) {
    if (error != nullptr) {
      *error = QObject::tr("Address out of protocol range (0x0000..0xFFFF)");
    }
    return std::nullopt;
  }

  return static_cast<quint16>(offset);
}

}  // namespace

bool Hardware::erasePages(const FourWay& fourWay,
                          int startPage,
                          int stopPage,
                          const McuProfile& profile,
                          QString* error) const {
  if (startPage < 0 || stopPage < startPage) {
    if (error != nullptr) {
      *error = QObject::tr("Invalid page range");
    }
    return false;
  }

  for (int page = startPage; page < stopPage; ++page) {
    if (!erasePage(fourWay, page, profile, error)) {
      return false;
    }
  }

  return true;
}

bool Hardware::writePages(const FourWay& fourWay,
                          int startPage,
                          int stopPage,
                          int pageSize,
                          const QByteArray& image,
                          const McuProfile& profile,
                          QString* error) const {
  Q_UNUSED(profile);

  if (startPage < 0 || stopPage < startPage || pageSize <= 0) {
    if (error != nullptr) {
      *error = QObject::tr("Invalid write pages arguments");
    }
    return false;
  }

  for (int page = startPage; page < stopPage; ++page) {
    const int offset = page * pageSize;
    if (offset + pageSize > image.size()) {
      if (error != nullptr) {
        *error = QObject::tr("Image is too small for requested page range");
      }
      return false;
    }

    int pageOffset = 0;
    while (pageOffset < pageSize) {
      const int chunkSize = qMin(256, pageSize - pageOffset);
      const auto address = toAddress(offset + pageOffset, error);
      if (!address.has_value()) {
        return false;
      }

      const QByteArray data = image.mid(offset + pageOffset, chunkSize);
      if (!writeChunk(fourWay, address.value(), data, error)) {
        return false;
      }

      pageOffset += chunkSize;
    }
  }

  return true;
}

bool Hardware::verifyPages(const FourWay& fourWay,
                           int startPage,
                           int stopPage,
                           int pageSize,
                           const QByteArray& image,
                           const McuProfile& profile,
                           QString* error) const {
  Q_UNUSED(profile);

  if (startPage < 0 || stopPage < startPage || pageSize <= 0) {
    if (error != nullptr) {
      *error = QObject::tr("Invalid verify pages arguments");
    }
    return false;
  }

  for (int page = startPage; page < stopPage; ++page) {
    const int offset = page * pageSize;
    if (offset + pageSize > image.size()) {
      if (error != nullptr) {
        *error = QObject::tr("Image is too small for requested page range");
      }
      return false;
    }

    int pageOffset = 0;
    while (pageOffset < pageSize) {
      const int chunkSize = qMin(256, pageSize - pageOffset);
      const auto address = toAddress(offset + pageOffset, error);
      if (!address.has_value()) {
        return false;
      }

      const auto actual = readChunk(fourWay, address.value(), chunkSize, error);
      if (!actual.has_value()) {
        return false;
      }

      const QByteArray expected = image.mid(offset + pageOffset, chunkSize);
      if (actual.value() != expected) {
        if (error != nullptr) {
          *error = QObject::tr("Verify mismatch on page %1").arg(page);
        }
        return false;
      }

      pageOffset += chunkSize;
    }
  }

  return true;
}

std::unique_ptr<Hardware> createHardware(quint8 interfaceMode) {
  if (interfaceMode == static_cast<quint8>(fourway::Mode::SiLBLB) ||
      interfaceMode == static_cast<quint8>(fourway::Mode::SiLC2)) {
    return std::make_unique<SilabsHardware>();
  }

  if (interfaceMode == static_cast<quint8>(fourway::Mode::ARMBLB)) {
    return std::make_unique<ArmHardware>();
  }

  return nullptr;
}

}  // namespace hardware
