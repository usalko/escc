#include <QObject>
#include <QtTest>

#include "utils/Convert.hpp"

class ConvertTests final : public QObject {
  Q_OBJECT

 private slots:
  void arrayToObject_mapsNumericStringAndMelody();
  void objectToArray_serializesBackToLayout();
  void asciiHelpers_roundTrip();
};

void ConvertTests::arrayToObject_mapsNumericStringAndMelody() {
  convert::Layout layout;
  layout.insert("BEEP_STRENGTH", convert::LayoutEntry{1, 0});
  layout.insert("MOTOR_KV", convert::LayoutEntry{2, 1});
  layout.insert("NAME", convert::LayoutEntry{5, 3});
  layout.insert("STARTUP_MELODY", convert::LayoutEntry{4, 8});

  QByteArray settings(12, '\0');
  settings[0] = static_cast<char>(7);
  settings[1] = static_cast<char>(0x12);
  settings[2] = static_cast<char>(0x34);
  settings.replace(3, 5, QByteArray("ESC  "));
  settings[8] = static_cast<char>(1);
  settings[9] = static_cast<char>(2);
  settings[10] = static_cast<char>(255);
  settings[11] = static_cast<char>(0);

  const QVariantMap object = convert::arrayToSettingsObject(settings, layout);

  QCOMPARE(object.value("BEEP_STRENGTH").toUInt(), static_cast<uint>(7));
  QCOMPARE(object.value("MOTOR_KV").toUInt(), static_cast<uint>(0x1234));
  QCOMPARE(object.value("NAME").toString(), QStringLiteral("ESC"));

  const QVariantList melody = object.value("STARTUP_MELODY").toList();
  QCOMPARE(melody.size(), 4);
  QCOMPARE(melody.at(0).toUInt(), static_cast<uint>(1));
  QCOMPARE(melody.at(2).toUInt(), static_cast<uint>(255));
}

void ConvertTests::objectToArray_serializesBackToLayout() {
  convert::Layout layout;
  layout.insert("BEEP_STRENGTH", convert::LayoutEntry{1, 0});
  layout.insert("MOTOR_KV", convert::LayoutEntry{2, 1});
  layout.insert("NAME", convert::LayoutEntry{5, 3});
  layout.insert("STARTUP_MELODY", convert::LayoutEntry{4, 8});

  QVariantMap settings;
  settings.insert("BEEP_STRENGTH", 9);
  settings.insert("MOTOR_KV", 4660);
  settings.insert("NAME", QStringLiteral("AB"));
  settings.insert("STARTUP_MELODY", QVariantList{257, 2, 3});

  const QByteArray encoded = convert::objectToSettingsArray(settings, layout, 12);

  QCOMPARE(static_cast<quint8>(encoded.at(0)), static_cast<quint8>(9));
  QCOMPARE(static_cast<quint8>(encoded.at(1)), static_cast<quint8>(0x12));
  QCOMPARE(static_cast<quint8>(encoded.at(2)), static_cast<quint8>(0x34));
  QCOMPARE(encoded.mid(3, 5), QByteArray("AB   "));
  QCOMPARE(static_cast<quint8>(encoded.at(8)), static_cast<quint8>(1));
  QCOMPARE(static_cast<quint8>(encoded.at(9)), static_cast<quint8>(2));
  QCOMPARE(static_cast<quint8>(encoded.at(10)), static_cast<quint8>(3));
  QCOMPARE(static_cast<quint8>(encoded.at(11)), static_cast<quint8>(0));
}

void ConvertTests::asciiHelpers_roundTrip() {
  const QString text = QStringLiteral("BLHeli");
  const QByteArray bytes = convert::asciiToBuffer(text);
  QCOMPARE(bytes, QByteArray("BLHeli"));
  QCOMPARE(convert::bufferToAscii(bytes), text);
}

QTEST_APPLESS_MAIN(ConvertTests)
#include "test_convert.moc"
