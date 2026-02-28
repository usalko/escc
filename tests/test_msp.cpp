#include <QObject>
#include <QtTest>

#include "core/Msp.hpp"

class MspTests final : public QObject {
  Q_OBJECT

 private slots:
  void encodeV1_buildsFrame();
  void encodeV2_buildsFrame();
  void decode_readsV1Frame();
  void decode_readsV2Frame();
};

void MspTests::encodeV1_buildsFrame() {
  const QByteArray payload = QByteArray::fromHex("0102");
  const QByteArray frame = msp::encodeV1(msp::ApiVersion, payload);

  QCOMPARE(frame.left(3), QByteArray("$M<"));
  QCOMPARE(static_cast<quint8>(frame.at(3)), static_cast<quint8>(2));
  QCOMPARE(static_cast<quint8>(frame.at(4)), static_cast<quint8>(1));
  QCOMPARE(frame.mid(5, 2), payload);
}

void MspTests::encodeV2_buildsFrame() {
  const QByteArray payload = QByteArray::fromHex("A1B2C3");
  const QByteArray frame = msp::encodeV2(msp::SendDshotCommand, payload, 0x10);

  QCOMPARE(frame.left(3), QByteArray("$X<"));
  QCOMPARE(static_cast<quint8>(frame.at(3)), static_cast<quint8>(0x10));
  QCOMPARE(static_cast<quint8>(frame.at(4)), static_cast<quint8>(0x03));
  QCOMPARE(static_cast<quint8>(frame.at(5)), static_cast<quint8>(0x30));
  QCOMPARE(static_cast<quint8>(frame.at(6)), static_cast<quint8>(3));
  QCOMPARE(static_cast<quint8>(frame.at(7)), static_cast<quint8>(0));
  QCOMPARE(frame.mid(8, 3), payload);
}

void MspTests::decode_readsV1Frame() {
  const QByteArray payload = QByteArray::fromHex("1122");
  const QByteArray frame = msp::encodeV1(msp::Status, payload);

  msp::Frame decoded;
  int consumed = 0;

  QVERIFY(msp::decode(frame, &decoded, &consumed));
  QCOMPARE(consumed, frame.size());
  QCOMPARE(decoded.command, static_cast<quint16>(msp::Status));
  QCOMPARE(decoded.payload, payload);
  QCOMPARE(decoded.v2, false);
}

void MspTests::decode_readsV2Frame() {
  const QByteArray payload = QByteArray::fromHex("99AA");
  const QByteArray frame = msp::encodeV2(msp::SendDshotCommand, payload, 0x00);

  msp::Frame decoded;
  int consumed = 0;

  QVERIFY(msp::decode(frame, &decoded, &consumed));
  QCOMPARE(consumed, frame.size());
  QCOMPARE(decoded.command, static_cast<quint16>(msp::SendDshotCommand));
  QCOMPARE(decoded.payload, payload);
  QCOMPARE(decoded.v2, true);
}

QTEST_APPLESS_MAIN(MspTests)
#include "test_msp.moc"
