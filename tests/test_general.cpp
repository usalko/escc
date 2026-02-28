#include <QObject>
#include <QtTest>

#include "utils/General.hpp"

class GeneralTests final : public QObject {
  Q_OBJECT

 private slots:
  void compare_comparesByteArrays();
  void retry_retriesUntilSuccess();
  void retry_failsAfterMaxRetries();
  void findMcu_findsBySignature();
  void isValidFlash_checksBlheliJumpPattern();
};

void GeneralTests::compare_comparesByteArrays() {
  QVERIFY(general::compare(QByteArray::fromHex("0102"), QByteArray::fromHex("0102")));
  QVERIFY(!general::compare(QByteArray::fromHex("0102"), QByteArray::fromHex("0103")));
}

void GeneralTests::retry_retriesUntilSuccess() {
  int calls = 0;
  QString error;
  int attempts = 0;

  const bool ok = general::retry(
      [&calls](QString* attemptError) {
        ++calls;
        if (calls < 3) {
          if (attemptError != nullptr) {
            *attemptError = QStringLiteral("temporary");
          }
          return false;
        }

        return true;
      },
      3,
      0,
      &error,
      &attempts);

  QVERIFY(ok);
  QCOMPARE(calls, 3);
  QCOMPARE(attempts, 3);
  QVERIFY(error.isEmpty());
}

void GeneralTests::retry_failsAfterMaxRetries() {
  QString error;
  int attempts = 0;

  const bool ok = general::retry(
      [](QString* attemptError) {
        if (attemptError != nullptr) {
          *attemptError = QStringLiteral("final failure");
        }
        return false;
      },
      2,
      0,
      &error,
      &attempts);

  QVERIFY(!ok);
  QCOMPARE(attempts, 2);
  QCOMPARE(error, QStringLiteral("final failure"));
}

void GeneralTests::findMcu_findsBySignature() {
  QVector<hardware::McuProfile> list;

  hardware::McuProfile a;
  a.name = QStringLiteral("A");
  a.signature = 0xE8B2;
  list.push_back(a);

  hardware::McuProfile b;
  b.name = QStringLiteral("B");
  b.signature = 0x1F06;
  list.push_back(b);

  const auto found = general::findMcu(0x1F06, list);
  QVERIFY(found.has_value());
  QCOMPARE(found->name, QStringLiteral("B"));
}

void GeneralTests::isValidFlash_checksBlheliJumpPattern() {
  const QByteArray valid = QByteArray::fromHex("0219FD112233");
  const QByteArray invalidJump = QByteArray::fromHex("FFFFFF112233");

  QVERIFY(general::isValidFlash(QStringLiteral("EFM8#BLHELI#"), valid));
  QVERIFY(!general::isValidFlash(QStringLiteral("EFM8"), valid));
  QVERIFY(!general::isValidFlash(QStringLiteral("EFM8#BLHELI#"), invalidJump));
}

QTEST_APPLESS_MAIN(GeneralTests)
#include "test_general.moc"
