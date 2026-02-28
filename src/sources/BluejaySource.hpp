#pragma once

#include <QStringList>

#include "GithubSource.hpp"

namespace sources {

class BluejaySource final : public GithubSource {
 public:
  BluejaySource(QJsonObject eeprom,
                QJsonObject settingsDescriptions,
                QJsonObject escs,
                QStringList blacklist = {});

  QString buildDisplayName(const QJsonObject& firmware) const override;
  QJsonArray getVersions(QString* error = nullptr) override;

  QString getFirmwareUrl(const QString& escKey,
                         const QString& version,
                         int pwm,
                         const QString& baseUrl) const;

  bool isValidName(const QString& name) const override;
  bool canMigrateTo(const QString& name) const override;
  QVector<int> getPwmForVersion(const QString& version) const;
  QStringList getSkipSettingsForLayouts(int oldLayout, int newLayout) const;
  QStringList getGroupOrder() const override;

 private:
  QStringList blacklist_;
};

}  // namespace sources
