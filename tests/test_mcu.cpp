#include <QObject>
#include <QtTest>

#include "hardware/Mcu.hpp"

class McuTests final : public QObject {
  Q_OBJECT

 private slots:
  void findMcuProfile_findsKnownSilabs();
  void findMcuProfile_findsKnownArm();
  void findMcuProfile_returnsEmptyForUnknown();
};

void McuTests::findMcuProfile_findsKnownSilabs() {
  const auto profile = hardware::findMcuProfile(1, 0xE8B2);
  QVERIFY(profile.has_value());
  QCOMPARE(profile->name, QString("EFM8BB21x"));
  QVERIFY(profile->eepromOffset.has_value());
  QCOMPARE(profile->eepromOffset.value(), static_cast<quint32>(0x1A00));
  QCOMPARE(profile->pageSize, 512);
}

void McuTests::findMcuProfile_findsKnownArm() {
  const auto profile = hardware::findMcuProfile(4, 0x1F06);
  QVERIFY(profile.has_value());
  QCOMPARE(profile->name, QString("STM32F051"));
  QVERIFY(profile->eepromOffset.has_value());
  QCOMPARE(profile->eepromOffset.value(), static_cast<quint32>(0x7C00));
  QCOMPARE(profile->pageSize, 1024);
}

void McuTests::findMcuProfile_returnsEmptyForUnknown() {
  const auto missing = hardware::findMcuProfile(1, 0xFFFF);
  QVERIFY(!missing.has_value());
}

QTEST_APPLESS_MAIN(McuTests)
#include "test_mcu.moc"
