#pragma once

#include <QStringList>

#include "Source.hpp"

namespace sources {

class GithubSource : public Source {
 public:
  GithubSource(QString name, QJsonObject eeprom, QJsonObject settingsDescriptions, QJsonObject escs);
  ~GithubSource() override = default;

  bool getRemoteVersionsList(const QString& repo,
                             QJsonArray* outVersions,
                             const QStringList& blacklist = {},
                             int amount = 100,
                             QString* error = nullptr);

    void setApiBaseUrl(const QString& apiBaseUrl);
    void setWebBaseUrl(const QString& webBaseUrl);

 protected:
    QString apiBaseUrl_{QStringLiteral("https://api.github.com/repos")};
    QString webBaseUrl_{QStringLiteral("https://github.com")};
};

}  // namespace sources
