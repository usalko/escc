#include <QObject>
#include <QtTest>

#include "sources/SourceDataLoader.hpp"

class SourceDataLoaderTests final : public QObject {
  Q_OBJECT

 private slots:
  void loadsAllSourceJsonAsObjects();
  void failsOnMissingFile();
};

void SourceDataLoaderTests::loadsAllSourceJsonAsObjects() {
  const QString base = QString::fromLatin1(QT_TESTCASE_SOURCEDIR) + QStringLiteral("/../resources/sources");

  const QStringList files = {
      QStringLiteral("am32/eeprom.json"),
      QStringLiteral("am32/settings.json"),
      QStringLiteral("bluejay/eeprom.json"),
      QStringLiteral("bluejay/settings.json"),
      QStringLiteral("blheli/eeprom.json"),
      QStringLiteral("blheli/settings.json"),
      QStringLiteral("blhelis/eeprom.json"),
      QStringLiteral("blhelis/settings.json"),
  };

  for (const auto& relative : files) {
    QJsonObject object;
    QString error;
    QVERIFY2(sources::SourceDataLoader::loadJsonObject(base + QStringLiteral("/") + relative,
                                                        &object,
                                                        &error),
             qPrintable(error));
    QVERIFY(!object.isEmpty());
  }
}

void SourceDataLoaderTests::failsOnMissingFile() {
  QJsonDocument doc;
  QString error;
  QVERIFY(!sources::SourceDataLoader::loadJsonDocument(QStringLiteral("/non/existing/source.json"), &doc, &error));
  QVERIFY(!error.isEmpty());
}

QTEST_MAIN(SourceDataLoaderTests)
#include "test_source_data_loader.moc"
