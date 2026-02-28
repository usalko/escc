#include <QtTest>

#include "core/FourWay.hpp"

class FourWayConstantsTests final : public QObject {
  Q_OBJECT

 private slots:
  void commandValues_matchProtocol();
  void ackValues_matchProtocol();
  void modeHelpers_identifyFamilies();
};

void FourWayConstantsTests::commandValues_matchProtocol() {
  QCOMPARE(static_cast<quint8>(fourway::Command::InterfaceTestAlive), static_cast<quint8>(0x30));
  QCOMPARE(static_cast<quint8>(fourway::Command::ProtocolGetVersion), static_cast<quint8>(0x31));
  QCOMPARE(static_cast<quint8>(fourway::Command::InterfaceGetName), static_cast<quint8>(0x32));
  QCOMPARE(static_cast<quint8>(fourway::Command::InterfaceGetVersion), static_cast<quint8>(0x33));
  QCOMPARE(static_cast<quint8>(fourway::Command::InterfaceExit), static_cast<quint8>(0x34));
  QCOMPARE(static_cast<quint8>(fourway::Command::DeviceInitFlash), static_cast<quint8>(0x37));
  QCOMPARE(static_cast<quint8>(fourway::Command::DeviceRead), static_cast<quint8>(0x3A));
  QCOMPARE(static_cast<quint8>(fourway::Command::DeviceWrite), static_cast<quint8>(0x3B));
  QCOMPARE(static_cast<quint8>(fourway::Command::InterfaceSetMode), static_cast<quint8>(0x3F));
}

void FourWayConstantsTests::ackValues_matchProtocol() {
  QCOMPARE(static_cast<quint8>(fourway::Ack::Ok), static_cast<quint8>(0x00));
  QCOMPARE(static_cast<quint8>(fourway::Ack::InterfaceInvalidCommand), static_cast<quint8>(0x02));
  QCOMPARE(static_cast<quint8>(fourway::Ack::InterfaceInvalidCrc), static_cast<quint8>(0x03));
  QCOMPARE(static_cast<quint8>(fourway::Ack::DeviceCommandFailed), static_cast<quint8>(0x06));
  QCOMPARE(static_cast<quint8>(fourway::Ack::DeviceGeneralError), static_cast<quint8>(0x0F));
}

void FourWayConstantsTests::modeHelpers_identifyFamilies() {
  QVERIFY(fourway::isSilabsMode(fourway::Mode::SiLC2));
  QVERIFY(fourway::isSilabsMode(fourway::Mode::SiLBLB));
  QVERIFY(!fourway::isSilabsMode(fourway::Mode::AtmBLB));

  QVERIFY(fourway::isAtmelMode(fourway::Mode::AtmBLB));
  QVERIFY(fourway::isAtmelMode(fourway::Mode::AtmSK));
  QVERIFY(!fourway::isAtmelMode(fourway::Mode::ARMBLB));

  QCOMPARE(fourway::kSilabsModes.size(), static_cast<size_t>(2));
  QCOMPARE(fourway::kAtmelModes.size(), static_cast<size_t>(2));
}

QTEST_APPLESS_MAIN(FourWayConstantsTests)
#include "test_fourway_constants.moc"
