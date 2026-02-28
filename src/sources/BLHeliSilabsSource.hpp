#pragma once

#include "BLHeliSource.hpp"

namespace sources {

class BLHeliSilabsSource final : public BLHeliSource {
 public:
  BLHeliSilabsSource(QJsonObject eeprom,
                     QJsonObject settingsDescriptions,
                     QJsonObject escs,
                     QJsonArray versions);

  QJsonArray getVersions(QString* error = nullptr) override;

 private:
  QJsonArray versions_;
};

}  // namespace sources
