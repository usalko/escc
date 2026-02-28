#pragma once

#include "Source.hpp"

namespace sources {

class BLHeliSource : public Source {
 public:
  BLHeliSource(QString name, QJsonObject eeprom, QJsonObject settingsDescriptions, QJsonObject escs);
  ~BLHeliSource() override = default;

  QString buildDisplayName(const QJsonObject& firmware) const override;

  QString getFirmwareUrl(const QString& escKey,
                         int mode,
                         const QString& urlPattern) const;
};

}  // namespace sources
