#include "BLHeliSSource.hpp"

namespace sources {

BLHeliSSource::BLHeliSSource(QJsonObject eeprom,
                             QJsonObject settingsDescriptions,
                             QJsonObject escs,
                             QJsonArray versions)
    : BLHeliSource(QStringLiteral("BLHeli_S"),
                   std::move(eeprom),
                   std::move(settingsDescriptions),
                   std::move(escs)),
      versions_(std::move(versions)) {}

QJsonArray BLHeliSSource::getVersions(QString* error) {
  Q_UNUSED(error);
  return versions_;
}

}  // namespace sources
