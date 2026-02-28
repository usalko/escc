#pragma once

#include "BLHeliSource.hpp"

namespace sources {

class BLHeliAtmelSource final : public BLHeliSource {
 public:
  BLHeliAtmelSource(QJsonObject eeprom,
                    QJsonObject settingsDescriptions,
                    QJsonObject escs,
                    QJsonArray versions);

  QJsonArray getVersions(QString* error = nullptr) override;

 private:
  QJsonArray versions_;
};

}  // namespace sources
