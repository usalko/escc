#include <QObject>
#include <QtTest>

#include "models/SettingsModel.hpp"

class SettingsModelTests final : public QObject {
  Q_OBJECT

 private slots:
  void mergesCommonAndIndividualWithOverride();
  void setValueForKey_updatesRequestedScope();
  void clear_resetsModel();
};

void SettingsModelTests::mergesCommonAndIndividualWithOverride() {
  models::SettingsModel model;

  model.setCommonSettings({
      {QStringLiteral("MOTOR_TIMING"), 16},
      {QStringLiteral("PWM_FREQUENCY"), 48},
  });
  model.setIndividualSettings({
      {QStringLiteral("PWM_FREQUENCY"), 24},
      {QStringLiteral("STARTUP_POWER"), 8},
  });

  QCOMPARE(model.rowCount(), 3);
  QCOMPARE(model.valueForKey(QStringLiteral("MOTOR_TIMING")).toInt(), 16);
  QCOMPARE(model.valueForKey(QStringLiteral("PWM_FREQUENCY")).toInt(), 24);

  bool foundOverride = false;
  for (int row = 0; row < model.rowCount(); ++row) {
    const QModelIndex idx = model.index(row, 0);
    const QString key = model.data(idx, models::SettingsModel::KeyRole).toString();
    if (key == QStringLiteral("PWM_FREQUENCY")) {
      foundOverride = true;
      QCOMPARE(model.data(idx, models::SettingsModel::ValueRole).toInt(), 24);
      QCOMPARE(model.data(idx, models::SettingsModel::ScopeRole).toString(),
               QStringLiteral("individual"));
    }
  }

  QVERIFY(foundOverride);
}

void SettingsModelTests::setValueForKey_updatesRequestedScope() {
  models::SettingsModel model;

  QVERIFY(model.setValueForKey(QStringLiteral("MOTOR_TIMING"), 16, false));
  QCOMPARE(model.commonSettings().value(QStringLiteral("MOTOR_TIMING")).toInt(), 16);

  QVERIFY(model.setValueForKey(QStringLiteral("MOTOR_TIMING"), 22, true));
  QCOMPARE(model.individualSettings().value(QStringLiteral("MOTOR_TIMING")).toInt(), 22);
  QCOMPARE(model.valueForKey(QStringLiteral("MOTOR_TIMING")).toInt(), 22);

  QVERIFY(!model.setValueForKey(QStringLiteral("   "), 10, true));
}

void SettingsModelTests::clear_resetsModel() {
  models::SettingsModel model;
  model.setCommonSettings({{QStringLiteral("PWM_FREQUENCY"), 48}});
  model.setIndividualSettings({{QStringLiteral("STARTUP_POWER"), 9}});

  QVERIFY(model.rowCount() > 0);
  model.clear();

  QCOMPARE(model.rowCount(), 0);
  QVERIFY(model.commonSettings().isEmpty());
  QVERIFY(model.individualSettings().isEmpty());
}

QTEST_MAIN(SettingsModelTests)
#include "test_settings_model.moc"
