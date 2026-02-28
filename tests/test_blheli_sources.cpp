#include <QObject>
#include <QtTest>

#include "sources/BLHeliAtmelSource.hpp"
#include "sources/BLHeliSSource.hpp"
#include "sources/BLHeliSilabsSource.hpp"

namespace {

class TestBLHeliSource final : public sources::BLHeliSource {
 public:
  TestBLHeliSource(QJsonObject escs)
      : BLHeliSource(QStringLiteral("BLHeli"), QJsonObject{}, QJsonObject{}, std::move(escs)) {}

  QJsonArray getVersions(QString* error = nullptr) override {
    Q_UNUSED(error);
    return {};
  }
};

}  // namespace

class BLHeliSourcesTests final : public QObject {
  Q_OBJECT

 private slots:
  void blheliDisplayName_handlesRevisionMistagAndFailure();
  void blheliFirmwareUrl_formatsLayoutAndMode();
  void blheliSReturnsConfiguredVersions();
  void blheliAtmelReturnsConfiguredVersions();
  void blheliSilabsReturnsConfiguredVersions();
};

void BLHeliSourcesTests::blheliDisplayName_handlesRevisionMistagAndFailure() {
  const QJsonObject escs{{QStringLiteral("layouts"),
                          QJsonObject{{QStringLiteral("ESC"), QJsonObject{{QStringLiteral("name"), QStringLiteral("A B")}}}}}};
  TestBLHeliSource source(escs);

  QJsonObject firmware{{QStringLiteral("make"), QStringLiteral("MAKE")},
                       {QStringLiteral("settings"),
                        QJsonObject{{QStringLiteral("MAIN_REVISION"), 1},
                                    {QStringLiteral("SUB_REVISION"), 100}}}};
  QCOMPARE(source.buildDisplayName(firmware), QStringLiteral("MAKE - BLHeli, 1.100"));

  firmware.insert(QStringLiteral("actualMake"), QStringLiteral("wrong tag"));
  QCOMPARE(source.buildDisplayName(firmware),
           QStringLiteral("MAKE (Probably mistagged: wrong tag) - BLHeli, 1.100"));

  firmware.insert(QStringLiteral("settings"), QJsonObject{{QStringLiteral("NAME"), QStringLiteral("**FLASH*FAILED**")}});
  QCOMPARE(source.buildDisplayName(firmware), QStringLiteral("**FLASH*FAILED**"));
}

void BLHeliSourcesTests::blheliFirmwareUrl_formatsLayoutAndMode() {
  const QJsonObject escs{{QStringLiteral("layouts"),
                          QJsonObject{{QStringLiteral("ESC"), QJsonObject{{QStringLiteral("name"), QStringLiteral("AFRO 12A")}}}}}};
  TestBLHeliSource source(escs);

  const QString url = source.getFirmwareUrl(QStringLiteral("ESC"),
                                            23,
                                            QStringLiteral("https://example.com/{0}/{1}.hex"));
  QCOMPARE(url, QStringLiteral("https://example.com/AFRO_12A/23.hex"));
}

void BLHeliSourcesTests::blheliSReturnsConfiguredVersions() {
  const QJsonArray versions{QJsonObject{{QStringLiteral("key"), QStringLiteral("16.7")},
                                        {QStringLiteral("url"), QStringLiteral("https://example/{0}.hex")}}};

  sources::BLHeliSSource source(QJsonObject{}, QJsonObject{}, QJsonObject{}, versions);
  QString error;
  const QJsonArray actual = source.getVersions(&error);
  QVERIFY(error.isEmpty());
  QCOMPARE(actual.size(), 1);
  QCOMPARE(actual.at(0).toObject().value(QStringLiteral("key")).toString(), QStringLiteral("16.7"));
}

void BLHeliSourcesTests::blheliAtmelReturnsConfiguredVersions() {
  const QJsonArray versions{QJsonObject{{QStringLiteral("key"), QStringLiteral("14.9")},
                                        {QStringLiteral("url"), QStringLiteral("https://example/{0}_{1}.hex")}}};

  sources::BLHeliAtmelSource source(QJsonObject{}, QJsonObject{}, QJsonObject{}, versions);
  QString error;
  const QJsonArray actual = source.getVersions(&error);
  QVERIFY(error.isEmpty());
  QCOMPARE(actual.size(), 1);
  QCOMPARE(actual.at(0).toObject().value(QStringLiteral("key")).toString(), QStringLiteral("14.9"));
}

void BLHeliSourcesTests::blheliSilabsReturnsConfiguredVersions() {
  const QJsonArray versions{QJsonObject{{QStringLiteral("key"), QStringLiteral("16.6")},
                                        {QStringLiteral("url"), QStringLiteral("https://example/{0}.hex")}}};

  sources::BLHeliSilabsSource source(QJsonObject{}, QJsonObject{}, QJsonObject{}, versions);
  QString error;
  const QJsonArray actual = source.getVersions(&error);
  QVERIFY(error.isEmpty());
  QCOMPARE(actual.size(), 1);
  QCOMPARE(actual.at(0).toObject().value(QStringLiteral("key")).toString(), QStringLiteral("16.6"));
}

QTEST_MAIN(BLHeliSourcesTests)
#include "test_blheli_sources.moc"
