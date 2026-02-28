#include <QObject>
#include <QSignalSpy>
#include <QtTest>

#include "models/AppState.hpp"

class AppStateTests final : public QObject {
  Q_OBJECT

 private slots:
  void connected_emitsOnlyOnChange();
  void sections_emitSignalsAndStoreValues();
};

void AppStateTests::connected_emitsOnlyOnChange() {
  AppState state;

  QSignalSpy connectedSpy(&state, &AppState::connectedChanged);
  QSignalSpy dispatchSpy(&state, &AppState::stateChanged);

  state.setConnected(true);
  QCOMPARE(connectedSpy.count(), 1);
  QCOMPARE(dispatchSpy.count(), 1);
  QCOMPARE(dispatchSpy.at(0).at(0).toString(), QStringLiteral("connected"));

  state.setConnected(true);
  QCOMPARE(connectedSpy.count(), 1);
  QCOMPARE(dispatchSpy.count(), 1);
}

void AppStateTests::sections_emitSignalsAndStoreValues() {
  AppState state;

  QSignalSpy escsSpy(&state, &AppState::escsChanged);
  QSignalSpy settingsSpy(&state, &AppState::settingsChanged);
  QSignalSpy languageSpy(&state, &AppState::languageChanged);
  QSignalSpy logSpy(&state, &AppState::logChanged);
  QSignalSpy melodiesSpy(&state, &AppState::melodiesChanged);
  QSignalSpy cookiesSpy(&state, &AppState::cookiesChanged);
  QSignalSpy dispatchSpy(&state, &AppState::stateChanged);

  const QVariantList escs = {
      QVariantMap{{QStringLiteral("index"), 0},
                  {QStringLiteral("firmwareName"), QStringLiteral("Bluejay")},
                  {QStringLiteral("firmwareVersion"), QStringLiteral("0.22.2")}},
  };
  const QVariantMap settings = {
      {QStringLiteral("MOTOR_TIMING"), 16},
      {QStringLiteral("BIDIRECTIONAL_MODE"), true},
  };

  state.setEscs(escs);
  state.setSettings(settings);
  state.setLanguage(QStringLiteral("ru"));
  state.appendLog(QStringLiteral("Connected"));
  state.setMelodies({QStringLiteral("melody1"), QStringLiteral("melody2")});
  state.setCookiesAccepted(true);

  QCOMPARE(state.escs(), escs);
  QCOMPARE(state.settings(), settings);
  QCOMPARE(state.language(), QStringLiteral("ru"));
  QCOMPARE(state.log().size(), 1);
  QCOMPARE(state.log().at(0), QStringLiteral("Connected"));
  QCOMPARE(state.melodies(), QStringList({QStringLiteral("melody1"), QStringLiteral("melody2")}));
  QVERIFY(state.cookiesAccepted());

  QCOMPARE(escsSpy.count(), 1);
  QCOMPARE(settingsSpy.count(), 1);
  QCOMPARE(languageSpy.count(), 1);
  QCOMPARE(logSpy.count(), 1);
  QCOMPARE(melodiesSpy.count(), 1);
  QCOMPARE(cookiesSpy.count(), 1);

  QCOMPARE(dispatchSpy.count(), 6);
  QCOMPARE(dispatchSpy.at(0).at(0).toString(), QStringLiteral("escs"));
  QCOMPARE(dispatchSpy.at(1).at(0).toString(), QStringLiteral("settings"));
  QCOMPARE(dispatchSpy.at(2).at(0).toString(), QStringLiteral("language"));
  QCOMPARE(dispatchSpy.at(3).at(0).toString(), QStringLiteral("log"));
  QCOMPARE(dispatchSpy.at(4).at(0).toString(), QStringLiteral("melodies"));
  QCOMPARE(dispatchSpy.at(5).at(0).toString(), QStringLiteral("cookies"));
}

QTEST_MAIN(AppStateTests)
#include "test_app_state.moc"
