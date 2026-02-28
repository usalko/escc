#include <QObject>
#include <QtTest>

#include "utils/Flash.hpp"

class FlashTests final : public QObject {
  Q_OBJECT

 private slots:
  void parseHex_readsValidIntelHex();
  void parseHex_rejectsBadChecksum();
  void fillImage_placesBlocksWithOffset();
  void getInfo_setsModeFlags();
};

void FlashTests::parseHex_readsValidIntelHex() {
  const QString hex =
      ":020000040800F2\n"
      ":100000000102030405060708090A0B0C0D0E0F1068\n"
      ":0400000501020304ED\n"
      ":00000001FF\n";

  QString error;
  const auto parsed = flash::parseHex(hex, &error);

  QVERIFY(parsed.has_value());
  QVERIFY(error.isEmpty());
  QVERIFY(parsed->endOfFile);
  QCOMPARE(parsed->bytes, 16);
  QCOMPARE(parsed->startLinearAddress, static_cast<quint32>(0x01020304));
  QCOMPARE(parsed->data.size(), 1);
  QCOMPARE(parsed->data.at(0).address, static_cast<quint32>(0x08000000));
  QCOMPARE(parsed->data.at(0).data.size(), 16);
  QCOMPARE(static_cast<quint8>(parsed->data.at(0).data.at(0)), static_cast<quint8>(0x01));
}

void FlashTests::parseHex_rejectsBadChecksum() {
  const QString invalidHex =
      ":100000000102030405060708090A0B0C0D0E0F1067\n"
      ":00000001FF\n";

  QString error;
  const auto parsed = flash::parseHex(invalidHex, &error);
  QVERIFY(!parsed.has_value());
  QVERIFY(!error.isEmpty());
}

void FlashTests::fillImage_placesBlocksWithOffset() {
  flash::ParsedHex parsed;
  parsed.endOfFile = true;
  parsed.data.push_back(flash::HexBlock{0x08000000, QByteArray::fromHex("AABBCC")});
  parsed.data.push_back(flash::HexBlock{0x08000005, QByteArray::fromHex("0102")});

  const auto image = flash::fillImage(parsed, 8, 0x08000000);
  QVERIFY(image.has_value());
  QCOMPARE(image->size(), 8);
  QCOMPARE(image->toHex(), QByteArray("aabbccffff0102ff"));
}

void FlashTests::getInfo_setsModeFlags() {
  QString error;
  const auto silabs = flash::getInfo(QByteArray::fromHex("B2E80001"), &error);
  QVERIFY(silabs.has_value());
  QVERIFY(error.isEmpty());
  QVERIFY(silabs->isSiLabs);
  QVERIFY(!silabs->isAtmel);
  QVERIFY(!silabs->isArm);
  QCOMPARE(silabs->meta.signature, static_cast<quint16>(0xE8B2));

  const auto arm = flash::getInfo(QByteArray::fromHex("061F0004"), &error);
  QVERIFY(arm.has_value());
  QVERIFY(arm->isArm);
}

QTEST_APPLESS_MAIN(FlashTests)
#include "test_flash.moc"
