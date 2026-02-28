#pragma once

#include <memory>
#include <optional>

#include <QByteArray>
#include <QString>

#include "core/FourWay.hpp"
#include "hardware/Mcu.hpp"

namespace hardware {

class Hardware {
 public:
  virtual ~Hardware() = default;

  virtual bool erasePage(const FourWay& fourWay,
                         int page,
                         const McuProfile& profile,
                         QString* error = nullptr) const = 0;

  virtual std::optional<QByteArray> readChunk(const FourWay& fourWay,
                                              quint16 address,
                                              int length,
                                              QString* error = nullptr) const = 0;

  virtual bool writeChunk(const FourWay& fourWay,
                          quint16 address,
                          const QByteArray& data,
                          QString* error = nullptr) const = 0;

    bool erasePages(const FourWay& fourWay,
                                 int startPage,
                                 int stopPage,
                                 const McuProfile& profile,
                                 QString* error = nullptr) const;

    bool writePages(const FourWay& fourWay,
                                    int startPage,
                                    int stopPage,
                                    int pageSize,
                                    const QByteArray& image,
                                    const McuProfile& profile,
                                    QString* error = nullptr) const;

    bool verifyPages(const FourWay& fourWay,
                                     int startPage,
                                     int stopPage,
                                     int pageSize,
                                     const QByteArray& image,
                                     const McuProfile& profile,
                                     QString* error = nullptr) const;
};

std::unique_ptr<Hardware> createHardware(quint8 interfaceMode);

}  // namespace hardware
