#include "BLHeliSilabsSource.hpp"

namespace sources {

BLHeliSilabsSource::BLHeliSilabsSource(QJsonObject eeprom,
                                       QJsonObject settingsDescriptions,
                                       QJsonObject escs,
                                       QJsonArray versions)
    : BLHeliSource(QStringLiteral("BLHeli"),
                   std::move(eeprom),
                   std::move(settingsDescriptions),
                   std::move(escs)),
      versions_(std::move(versions)) {}

QJsonArray BLHeliSilabsSource::getVersions(QString* error) {
  Q_UNUSED(error);
  return versions_;
}

}  // namespace sources
