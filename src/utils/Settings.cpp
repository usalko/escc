#include "Settings.hpp"

namespace settings {

namespace {

constexpr auto kBaseGroup = "base";

QStringList fixedIndividualFields() {
  return {
      QStringLiteral("MAIN_REVISION"),
      QStringLiteral("SUB_REVISION"),
      QStringLiteral("LAYOUT"),
      QStringLiteral("LAYOUT_REVISION"),
      QStringLiteral("NAME"),
  };
}

std::optional<int> readInt(const QVariantMap& map, const QString& key) {
  if (!map.contains(key)) {
    return std::nullopt;
  }

  bool ok = false;
  const int value = map.value(key).toInt(&ok);
  if (!ok) {
    return std::nullopt;
  }

  return value;
}

bool containsSetting(const SettingsGroups& groups, const QString& settingName) {
  if (!groups.contains(kBaseGroup)) {
    return false;
  }

  const auto& base = groups.value(kBaseGroup);
  for (const auto& descriptor : base) {
    if (descriptor.name == settingName) {
      return true;
    }
  }

  return false;
}

}  // namespace

std::optional<EscData> getMaster(const QVector<EscData>& escs) {
  for (const auto& esc : escs) {
    if (esc.meta.available) {
      return esc;
    }
  }

  return std::nullopt;
}

QVariantMap getMasterSettings(const QVector<EscData>& escs) {
  const auto master = getMaster(escs);
  if (!master.has_value()) {
    return {};
  }

  return master->settings;
}

QStringList getIndividualSettingsDescriptions(
    const EscData& esc,
    const FirmwareIndividualDescriptions& individualDescriptionsByFirmware) {
  if (esc.firmwareName.isEmpty() || esc.layoutRevision <= 0) {
    return {};
  }

  if (!individualDescriptionsByFirmware.contains(esc.firmwareName)) {
    return {};
  }

  const auto revisions = individualDescriptionsByFirmware.value(esc.firmwareName);
  if (!revisions.contains(esc.layoutRevision)) {
    return {};
  }

  QStringList keep = fixedIndividualFields();
  const auto groups = revisions.value(esc.layoutRevision);
  for (auto it = groups.cbegin(); it != groups.cend(); ++it) {
    const auto& group = it.value();
    for (const auto& entry : group) {
      keep.push_back(entry.name);
    }
  }

  return keep;
}

QVariantMap getIndividualSettings(
    const EscData& esc,
    const FirmwareIndividualDescriptions& individualDescriptionsByFirmware) {
  QVariantMap individualSettings;
  if (esc.settings.isEmpty()) {
    return individualSettings;
  }

  const QStringList keep =
      getIndividualSettingsDescriptions(esc, individualDescriptionsByFirmware);

  for (const auto& settingName : keep) {
    if (esc.settings.contains(settingName)) {
      individualSettings.insert(settingName, esc.settings.value(settingName));
    }
  }

  return individualSettings;
}

bool canMigrate(const QString& settingName,
                const QVariantMap& from,
                const QVariantMap& to,
                const RevisionSettingsDescriptions& toSettingsDescriptions,
                const RevisionSettingsDescriptions& toIndividualSettingsDescriptions) {
  const auto fromMode = readInt(from, QStringLiteral("MODE"));
  const auto toMode = readInt(to, QStringLiteral("MODE"));
  if (!fromMode.has_value() || !toMode.has_value() || fromMode.value() != toMode.value()) {
    return false;
  }

  const auto fromLayoutRevision = readInt(from, QStringLiteral("LAYOUT_REVISION"));
  const auto toLayoutRevision = readInt(to, QStringLiteral("LAYOUT_REVISION"));
  if (!fromLayoutRevision.has_value() || !toLayoutRevision.has_value()) {
    return false;
  }

  if (!toSettingsDescriptions.contains(fromLayoutRevision.value()) ||
      !toSettingsDescriptions.contains(toLayoutRevision.value()) ||
      !toIndividualSettingsDescriptions.contains(fromLayoutRevision.value()) ||
      !toIndividualSettingsDescriptions.contains(toLayoutRevision.value())) {
    return false;
  }

  const auto fromCommon = toSettingsDescriptions.value(fromLayoutRevision.value());
  const auto toCommon = toSettingsDescriptions.value(toLayoutRevision.value());
  if (!fromCommon.contains(kBaseGroup) || !toCommon.contains(kBaseGroup)) {
    return false;
  }

  if (containsSetting(fromCommon, settingName) && containsSetting(toCommon, settingName)) {
    return true;
  }

  const auto fromIndividual = toIndividualSettingsDescriptions.value(fromLayoutRevision.value());
  const auto toIndividual = toIndividualSettingsDescriptions.value(toLayoutRevision.value());
  return containsSetting(fromIndividual, settingName) && containsSetting(toIndividual, settingName);
}

}  // namespace settings
