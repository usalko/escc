#include "BluejaySource.hpp"

#include <QRegularExpression>

namespace sources {
namespace {

constexpr auto kRepo = "bird-sanctuary/bluejay";

QVector<int> parseVersion(const QString& version) {
  QVector<int> out;
  const QString normalized = version.trimmed();
  const auto parts = normalized.split('.');
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

bool semverLessThan(const QString& a, const QString& b) {
  const auto av = parseVersion(a);
  const auto bv = parseVersion(b);
  if (av.size() < 3 || bv.size() < 3) {
    return false;
  }

  for (int i = 0; i < 3; ++i) {
    if (av[i] < bv[i]) {
      return true;
    }
    if (av[i] > bv[i]) {
      return false;
    }
  }

  return false;
}

}  // namespace

BluejaySource::BluejaySource(QJsonObject eeprom,
                             QJsonObject settingsDescriptions,
                             QJsonObject escs,
                             QStringList blacklist)
    : GithubSource(QStringLiteral("Bluejay"),
                   std::move(eeprom),
                   std::move(settingsDescriptions),
                   std::move(escs)),
      blacklist_(std::move(blacklist)) {}

QString BluejaySource::buildDisplayName(const QJsonObject& firmware) const {
  const auto settings = firmware.value(QStringLiteral("settings")).toObject();
  const QString make = firmware.value(QStringLiteral("make")).toString();

  QString name = settings.value(QStringLiteral("NAME")).toString().trimmed();
  QString versionSuffix;
  QString version = QStringLiteral("Unsupported/Unrecognized");

  if (settings.contains(QStringLiteral("MAIN_REVISION")) &&
      settings.contains(QStringLiteral("SUB_REVISION"))) {
    const int mainRevision = settings.value(QStringLiteral("MAIN_REVISION")).toInt();
    const int subRevision = settings.value(QStringLiteral("SUB_REVISION")).toInt();

    if (mainRevision > 0 || subRevision >= 20) {
      versionSuffix = QStringLiteral(".0");

      const QRegularExpression regex(QStringLiteral("^([a-zA-Z]*)( (\\((.*)\\)))?$"));
      const auto match = regex.match(name);
      if (match.hasMatch()) {
        name = match.captured(1);
        const QString captured = match.captured(4);
        if (!captured.isEmpty()) {
          versionSuffix = captured;
          if (!versionSuffix.startsWith(' ') && !versionSuffix.startsWith('.')) {
            versionSuffix = QStringLiteral(".0 ") + versionSuffix;
          }
        }
      }
    }

    version = QStringLiteral("%1.%2%3").arg(mainRevision).arg(subRevision).arg(versionSuffix);
  }

  QString pwm;
  const int pwmFrequency = settings.value(QStringLiteral("PWM_FREQUENCY")).toInt(0xFF);
  if (pwmFrequency != 0 && pwmFrequency != 0xFF) {
    pwm = QStringLiteral(", %1kHz").arg(pwmFrequency);
    if (pwmFrequency == 192) {
      pwm = QStringLiteral(", Dynamic PWM");
    }
  }

  return QStringLiteral("%1 - %2, %3%4").arg(make, name, version, pwm);
}

QJsonArray BluejaySource::getVersions(QString* error) {
  QJsonArray versions;
  if (!getRemoteVersionsList(QString::fromLatin1(kRepo), &versions, blacklist_, 8, error)) {
    return {};
  }

  return versions;
}

QString BluejaySource::getFirmwareUrl(const QString& escKey,
                                      const QString& version,
                                      int pwm,
                                      const QString& baseUrl) const {
  const auto layouts = escs_.value(QStringLiteral("layouts")).toObject();
  const auto layout = layouts.value(escKey).toObject();
  QString name = layout.value(QStringLiteral("name")).toString();
  name.replace(QRegularExpression(QStringLiteral("[\\s-]")), QStringLiteral("_"));
  name = name.toUpper();

  if (version == QStringLiteral("test-melody-pwm")) {
    return QStringLiteral("%1%2_%3.hex").arg(baseUrl, name, version);
  }

  return QStringLiteral("%1%2_%3_%4.hex").arg(baseUrl, name).arg(pwm).arg(version);
}

bool BluejaySource::isValidName(const QString& name) const {
  static const QRegularExpression regex(QStringLiteral("Bluejay( \\(.*\\))?"));
  return regex.match(name).hasMatch();
}

bool BluejaySource::canMigrateTo(const QString& name) const {
  return isValidName(name);
}

QVector<int> BluejaySource::getPwmForVersion(const QString& version) const {
  if (semverLessThan(version, QStringLiteral("0.22.0"))) {
    return {24, 48, 96};
  }

  return {};
}

QStringList BluejaySource::getSkipSettingsForLayouts(int oldLayout, int newLayout) const {
  if (newLayout == oldLayout) {
    return {};
  }

  if (oldLayout < newLayout && newLayout == 207) {
    return {QStringLiteral("DITHERING"), QStringLiteral("TEMPERATURE_PROTECTION")};
  }

  if (oldLayout < newLayout && newLayout == 208) {
    return {
        QStringLiteral("DITHERING"),
        QStringLiteral("TEMPERATURE_PROTECTION"),
        QStringLiteral("STARTUP_POWER_MIN"),
        QStringLiteral("STARTUP_POWER_MAX"),
    };
  }

  return {};
}

QStringList BluejaySource::getGroupOrder() const {
  return {
      QStringLiteral("general"),
      QStringLiteral("bluejayBeacon"),
      QStringLiteral("bluejaySafety"),
      QStringLiteral("bluejayBrake"),
      QStringLiteral("bluejayExperimental"),
  };
}

}  // namespace sources
