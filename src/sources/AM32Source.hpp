#pragma once

#include "GithubSource.hpp"

namespace sources {

class AM32Source final : public GithubSource {
 public:
  AM32Source(QJsonObject eeprom, QJsonObject settingsDescriptions, QJsonObject escs);

  QString buildDisplayName(const QJsonObject& firmware) const override;
  QJsonArray getVersions(QString* error = nullptr) override;

  QString getFirmwareUrl(const QString& escKey,
                         const QString& version,
                         const QString& baseUrl,
                         const QJsonObject& escMeta = {}) const;

  QStringList getValidNames() const override;
  QStringList getGroupOrder() const override;

 private:
  static bool versionAtLeast(const QString& version, const QString& minimum);
  QString minVersion_{QStringLiteral("1.94")};
};

}  // namespace sources
