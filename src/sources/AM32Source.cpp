#include "AM32Source.hpp"

#include <QRegularExpression>

namespace sources {
namespace {

constexpr auto kRepo = "am32-firmware/AM32";

QVector<int> parseVersion(const QString& version) {
  QString normalized = version;
  if (normalized.startsWith('v')) {
    normalized.remove(0, 1);
  }

  const auto parts = normalized.split('.');
  QVector<int> out;
  for (const auto& part : parts) {
    bool ok = false;
    const int value = part.toInt(&ok);
    if (!ok) {
      return {};
    }
    out.push_back(value);
  }

  while (out.size() < 3) {
    out.push_back(0);
  }

  return out;
}

}  // namespace

AM32Source::AM32Source(QJsonObject eeprom, QJsonObject settingsDescriptions, QJsonObject escs)
    : GithubSource(QStringLiteral("AM32"),
                   std::move(eeprom),
                   std::move(settingsDescriptions),
                   std::move(escs)) {}

bool AM32Source::versionAtLeast(const QString& version, const QString& minimum) {
  const auto v = parseVersion(version);
  const auto m = parseVersion(minimum);
  if (v.size() < 3 || m.size() < 3) {
    return false;
  }

  for (int i = 0; i < 3; ++i) {
    if (v[i] > m[i]) {
      return true;
    }
    if (v[i] < m[i]) {
      return false;
    }
  }

  return true;
}

QString AM32Source::buildDisplayName(const QJsonObject& firmware) const {
  const auto settings = firmware.value(QStringLiteral("settings")).toObject();
  const QString make = firmware.value(QStringLiteral("make")).toString();

  QString revision = QStringLiteral("Unsupported/Unrecognized");
  if (settings.contains(QStringLiteral("MAIN_REVISION")) &&
      settings.contains(QStringLiteral("SUB_REVISION"))) {
    const int mainRevision = settings.value(QStringLiteral("MAIN_REVISION")).toInt();
    const int subRevision = settings.value(QStringLiteral("SUB_REVISION")).toInt();
    revision = QStringLiteral("%1.%2%3")
                   .arg(mainRevision)
                   .arg(subRevision > 9 ? QString() : QStringLiteral("0"))
                   .arg(subRevision);
  }

  if (make == QStringLiteral("NOT READY")) {
    revision = QStringLiteral("FLASH FIRMWARE");
  }

  return QStringLiteral("%1 - %2, %3").arg(make, name_, revision);
}

QJsonArray AM32Source::getVersions(QString* error) {
  QJsonArray releases;
  if (!getRemoteVersionsList(QString::fromLatin1(kRepo), &releases, {}, 100, error)) {
    return {};
  }

  QJsonArray filtered;
  for (const auto& item : releases) {
    const auto release = item.toObject();
    const QString key = release.value(QStringLiteral("key")).toString();
    if (versionAtLeast(key, minVersion_)) {
      filtered.push_back(release);
    }
  }

  return filtered;
}

QString AM32Source::getFirmwareUrl(const QString& escKey,
                                   const QString& version,
                                   const QString& baseUrl,
                                   const QJsonObject& escMeta) const {
  QString fileName = escMeta.value(QStringLiteral("fileName")).toString();
  if (fileName.isEmpty()) {
    const auto layouts = escs_.value(QStringLiteral("layouts")).toObject();
    fileName = layouts.value(escKey).toObject().value(QStringLiteral("fileName")).toString();
  }

  QString normalizedVersion = version;
  if (normalizedVersion.startsWith('v')) {
    normalizedVersion.remove(0, 1);
  }

  return QStringLiteral("%1AM32_%2_%3.hex").arg(baseUrl, fileName, normalizedVersion);
}

QStringList AM32Source::getValidNames() const {
  return escs_.value(QStringLiteral("layouts")).toObject().keys();
}

QStringList AM32Source::getGroupOrder() const {
  return {
      QStringLiteral("general"),
      QStringLiteral("am32motor"),
      QStringLiteral("am32pwm"),
      QStringLiteral("am32brake"),
      QStringLiteral("am32sine"),
  };
}

}  // namespace sources
