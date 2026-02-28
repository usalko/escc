#include <QObject>
#include <QtTest>

#include "utils/Settings.hpp"

class SettingsTests final : public QObject {
  Q_OBJECT

 private slots:
  void getMasterAndMasterSettings_chooseAvailableEsc();
  void getIndividualSettings_extractsOnlyAllowedFields();
  void canMigrate_checksCommonAndIndividualBases();
};

void SettingsTests::getMasterAndMasterSettings_chooseAvailableEsc() {
  const settings::EscData first{
      .firmwareName = QStringLiteral("Bluejay"),
      .layoutRevision = 1,
      .settings = QVariantMap{{"NAME", "ESC-1"}},
      .meta = settings::EscMeta{false},
  };
  const settings::EscData second{
      .firmwareName = QStringLiteral("Bluejay"),
      .layoutRevision = 1,
      .settings = QVariantMap{{"NAME", "ESC-2"}, {"MOTOR_KV", 2207}},
      .meta = settings::EscMeta{true},
  };

  const QVector<settings::EscData> escs{first, second};
  const auto master = settings::getMaster(escs);

  QVERIFY(master.has_value());
  QCOMPARE(master->settings.value("NAME").toString(), QStringLiteral("ESC-2"));

  const QVariantMap masterSettings = settings::getMasterSettings(escs);
  QCOMPARE(masterSettings.value("MOTOR_KV").toInt(), 2207);
}

void SettingsTests::getIndividualSettings_extractsOnlyAllowedFields() {
  settings::FirmwareIndividualDescriptions desc;
  settings::SettingsGroups groups;
  groups.insert(QStringLiteral("base"),
                settings::SettingGroup{{QStringLiteral("MOTOR_DIRECTION")},
                                       {QStringLiteral("STARTUP_POWER")}});
  desc.insert(QStringLiteral("Bluejay"), {{3, groups}});

  settings::EscData esc;
  esc.firmwareName = QStringLiteral("Bluejay");
  esc.layoutRevision = 3;
  esc.settings = QVariantMap{{"MAIN_REVISION", 1},
                             {"SUB_REVISION", 20},
                             {"LAYOUT", "X"},
                             {"LAYOUT_REVISION", 3},
                             {"NAME", "ESC_A"},
                             {"MOTOR_DIRECTION", 2},
                             {"STARTUP_POWER", 7},
                             {"UNRELATED", 999}};

  const QVariantMap result = settings::getIndividualSettings(esc, desc);
  QVERIFY(result.contains("MAIN_REVISION"));
  QVERIFY(result.contains("MOTOR_DIRECTION"));
  QVERIFY(!result.contains("UNRELATED"));
}

void SettingsTests::canMigrate_checksCommonAndIndividualBases() {
  settings::RevisionSettingsDescriptions common;
  settings::RevisionSettingsDescriptions individual;

  common.insert(10, settings::SettingsGroups{
                        {QStringLiteral("base"), settings::SettingGroup{{QStringLiteral("PWM")}}}});
  common.insert(11, settings::SettingsGroups{
                        {QStringLiteral("base"), settings::SettingGroup{{QStringLiteral("PWM")}}}});

  individual.insert(
      10,
      settings::SettingsGroups{{QStringLiteral("base"),
                                settings::SettingGroup{{QStringLiteral("STARTUP_POWER")}}}});
  individual.insert(
      11,
      settings::SettingsGroups{{QStringLiteral("base"),
                                settings::SettingGroup{{QStringLiteral("STARTUP_POWER")}}}});

  const QVariantMap from{{"MODE", 1}, {"LAYOUT_REVISION", 10}};
  const QVariantMap to{{"MODE", 1}, {"LAYOUT_REVISION", 11}};

  QVERIFY(settings::canMigrate("PWM", from, to, common, individual));
  QVERIFY(settings::canMigrate("STARTUP_POWER", from, to, common, individual));

  const QVariantMap otherMode{{"MODE", 2}, {"LAYOUT_REVISION", 11}};
  QVERIFY(!settings::canMigrate("PWM", from, otherMode, common, individual));
}

QTEST_APPLESS_MAIN(SettingsTests)
#include "test_settings.moc"
