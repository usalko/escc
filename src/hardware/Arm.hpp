#pragma once

#include "hardware/Hardware.hpp"

namespace hardware {

class ArmHardware final : public Hardware {
 public:
  bool erasePage(const FourWay& fourWay,
                 int page,
                 const McuProfile& profile,
                 QString* error = nullptr) const override;

  std::optional<QByteArray> readChunk(const FourWay& fourWay,
                                      quint16 address,
                                      int length,
                                      QString* error = nullptr) const override;

  bool writeChunk(const FourWay& fourWay,
                  quint16 address,
                  const QByteArray& data,
                  QString* error = nullptr) const override;
};

}  // namespace hardware
