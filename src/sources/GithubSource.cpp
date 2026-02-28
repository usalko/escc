#include "GithubSource.hpp"

#include <QUrl>

namespace sources {

GithubSource::GithubSource(QString name,
                           QJsonObject eeprom,
                           QJsonObject settingsDescriptions,
                           QJsonObject escs)
    : Source(std::move(name),
             std::move(eeprom),
             std::move(settingsDescriptions),
             std::move(escs)) {}

void GithubSource::setApiBaseUrl(const QString& apiBaseUrl) {
  apiBaseUrl_ = apiBaseUrl;
}

void GithubSource::setWebBaseUrl(const QString& webBaseUrl) {
  webBaseUrl_ = webBaseUrl;
}

bool GithubSource::getRemoteVersionsList(const QString& repo,
                                         QJsonArray* outVersions,
                                         const QStringList& blacklist,
                                         int amount,
                                         QString* error) {
  if (outVersions == nullptr) {
    if (error != nullptr) {
      *error = QStringLiteral("Output versions array is null");
    }
    return false;
  }

  if (repo.isEmpty()) {
    if (error != nullptr) {
      *error = QStringLiteral("Repository must not be empty");
    }
    return false;
  }

  QString apiBase = apiBaseUrl_;
  while (apiBase.endsWith('/')) {
    apiBase.chop(1);
  }

  QString webBase = webBaseUrl_;
  while (webBase.endsWith('/')) {
    webBase.chop(1);
  }

  const QString url = QStringLiteral("%1/%2/releases?per_page=%3&page=1")
                          .arg(apiBase, repo)
                          .arg(amount);

  QJsonDocument json;
  if (!Source::getRemoteVersionsList(QUrl(url), &json, error, HttpClient::kOneDayMs)) {
    return false;
  }

  if (!json.isArray()) {
    if (error != nullptr) {
      *error = QStringLiteral("GitHub releases response is not an array");
    }
    return false;
  }

  QJsonArray validReleases;
  const QJsonArray releases = json.array();
  for (const auto& item : releases) {
    if (!item.isObject()) {
      continue;
    }

    const QJsonObject release = item.toObject();
    const QString tagName = release.value(QStringLiteral("tag_name")).toString();
    const QJsonArray assets = release.value(QStringLiteral("assets")).toArray();
    if (tagName.isEmpty() || assets.isEmpty() || blacklist.contains(tagName)) {
      continue;
    }

    QString displayName = release.value(QStringLiteral("name")).toString();
    if (displayName.isEmpty()) {
      displayName = tagName;
      if (displayName.startsWith(QLatin1Char('v'))) {
        displayName.remove(0, 1);
      }
    }

    QJsonObject normalized;
    normalized.insert(QStringLiteral("name"), displayName);
    normalized.insert(QStringLiteral("key"), tagName);
    normalized.insert(QStringLiteral("url"),
              QStringLiteral("%1/%2/releases/download/%3/").arg(webBase, repo, tagName));
    normalized.insert(
        QStringLiteral("releaseUrl"),
      QStringLiteral("%1/%2/releases/tag/%3/").arg(webBase, repo, tagName));
    normalized.insert(QStringLiteral("prerelease"),
                      release.value(QStringLiteral("prerelease")).toBool());
    normalized.insert(QStringLiteral("published_at"),
                      release.value(QStringLiteral("published_at")).toString());

    validReleases.push_back(normalized);
  }

  *outVersions = validReleases;
  return true;
}

}  // namespace sources
