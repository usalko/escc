#include "BLHeliSource.hpp"

#include <QRegularExpression>

namespace sources {

BLHeliSource::BLHeliSource(QString name,
                           QJsonObject eeprom,
                           QJsonObject settingsDescriptions,
                           QJsonObject escs)
    : Source(std::move(name),
             std::move(eeprom),
             std::move(settingsDescriptions),
             std::move(escs)) {}

QString BLHeliSource::buildDisplayName(const QJsonObject& firmware) const {
  constexpr auto kFlashFailedString = "**FLASH*FAILED**";

  const auto settings = firmware.value(QStringLiteral("settings")).toObject();
  QString make = firmware.value(QStringLiteral("make")).toString();

  QString revision = QStringLiteral("Unsupported/Unrecognized");
  if (settings.contains(QStringLiteral("MAIN_REVISION")) &&
      settings.contains(QStringLiteral("SUB_REVISION"))) {
    revision = QStringLiteral("%1.%2")
                   .arg(settings.value(QStringLiteral("MAIN_REVISION")).toInt())
                   .arg(settings.value(QStringLiteral("SUB_REVISION")).toInt());
  }

  const QString actualMake = firmware.value(QStringLiteral("actualMake")).toString();
  if (!actualMake.isEmpty()) {
    make += QStringLiteral(" (Probably mistagged: %1)").arg(actualMake);
  }

  const QString nameSetting = settings.value(QStringLiteral("NAME")).toString();
  if (nameSetting == QString::fromLatin1(kFlashFailedString)) {
    return QString::fromLatin1(kFlashFailedString);
  }

  return QStringLiteral("%1 - %2, %3").arg(make, name_, revision);
}

QString BLHeliSource::getFirmwareUrl(const QString& escKey,
                                     int mode,
                                     const QString& urlPattern) const {
  const auto layouts = escs_.value(QStringLiteral("layouts")).toObject();
  QString name = layouts.value(escKey).toObject().value(QStringLiteral("name")).toString();
  name.replace(QRegularExpression(QStringLiteral("[\\s-]")), QStringLiteral("_"));
  name = name.toUpper();

  QString formatted = urlPattern;
  formatted.replace(QStringLiteral("{0}"), name);
  formatted.replace(QStringLiteral("{1}"), QString::number(mode));
  return formatted;
}

}  // namespace sources
