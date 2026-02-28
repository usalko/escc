#pragma once

#include <optional>

#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QVector>

namespace settings {

struct EscMeta {
  bool available{false};
};

struct EscData {
  QString firmwareName;
  int layoutRevision{0};
  QVariantMap settings;
  EscMeta meta;
};

struct SettingDescriptor {
  QString name;
};

using SettingGroup = QVector<SettingDescriptor>;
using SettingsGroups = QMap<QString, SettingGroup>;
using RevisionSettingsDescriptions = QMap<int, SettingsGroups>;
using FirmwareIndividualDescriptions = QMap<QString, RevisionSettingsDescriptions>;

std::optional<EscData> getMaster(const QVector<EscData>& escs);
QVariantMap getMasterSettings(const QVector<EscData>& escs);

QStringList getIndividualSettingsDescriptions(
    const EscData& esc,
    const FirmwareIndividualDescriptions& individualDescriptionsByFirmware);
QVariantMap getIndividualSettings(const EscData& esc,
                                 const FirmwareIndividualDescriptions& individualDescriptionsByFirmware);

bool canMigrate(const QString& settingName,
                const QVariantMap& from,
                const QVariantMap& to,
                const RevisionSettingsDescriptions& toSettingsDescriptions,
                const RevisionSettingsDescriptions& toIndividualSettingsDescriptions);

}  // namespace settings
