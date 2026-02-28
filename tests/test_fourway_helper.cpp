#include <QObject>
#include <QtTest>

#include "utils/FourWayHelper.hpp"

class FourWayHelperTests final : public QObject {
  Q_OBJECT

 private slots:
  void commandToString_mapsKnownCommand();
  void ackToString_mapsKnownAck();
};

void FourWayHelperTests::commandToString_mapsKnownCommand() {
  const auto commandName =
      fourway_helper::commandToString(fourway::Command::DeviceInitFlash);

  QVERIFY(commandName.has_value());
  QCOMPARE(commandName.value(), QStringLiteral("DeviceInitFlash"));
}

void FourWayHelperTests::ackToString_mapsKnownAck() {
  const auto ackName = fourway_helper::ackToString(fourway::Ack::DeviceCommandFailed);

  QVERIFY(ackName.has_value());
  QCOMPARE(ackName.value(), QStringLiteral("DeviceCommandFailed"));
}

QTEST_APPLESS_MAIN(FourWayHelperTests)
#include "test_fourway_helper.moc"
