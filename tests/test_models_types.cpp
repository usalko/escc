#include <QObject>
#include <QtTest>

#include "models/Types.hpp"

class ModelTypesTests final : public QObject {
  Q_OBJECT

 private slots:
  void firmwareInfo_validationRequiresNameVersionPlatform();
  void flightControllerInfo_validationRequiresVariantAndVersion();
  void motorState_validationRequiresNonNegativeIndexAndSpeed();
  void escConfig_validationRequiresFirmwareAndIndex();
};

void ModelTypesTests::firmwareInfo_validationRequiresNameVersionPlatform() {
  models::FirmwareInfo info;
  QVERIFY(!info.isValid());

  info.name = QStringLiteral("Bluejay");
  info.version = QStringLiteral("0.22.2");
  info.platform = QStringLiteral("EFM8");
  QVERIFY(info.isValid());
}

void ModelTypesTests::flightControllerInfo_validationRequiresVariantAndVersion() {
  models::FlightControllerInfo info;
  info.apiVersion = QStringLiteral("1.46");
  info.boardName = QStringLiteral("STM32F4");
  QVERIFY(!info.isValid());

  info.variant = QStringLiteral("BTFL");
  info.version = QStringLiteral("4.5.0");
  QVERIFY(info.isValid());
}

void ModelTypesTests::motorState_validationRequiresNonNegativeIndexAndSpeed() {
  models::MotorState state;
  QVERIFY(!state.isValid());

  state.index = 0;
  state.speed = 1050;
  state.enabled = true;
  QVERIFY(state.isValid());

  state.speed = -1;
  QVERIFY(!state.isValid());
}

void ModelTypesTests::escConfig_validationRequiresFirmwareAndIndex() {
  models::EscConfig esc;
  QVERIFY(!esc.isValid());

  esc.index = 0;
  esc.name = QStringLiteral("ESC 1");
  esc.firmwareName = QStringLiteral("Bluejay");
  esc.firmwareVersion = QStringLiteral("0.22.2");
  esc.layout = QStringLiteral("A_H_20");
  esc.settings.insert(QStringLiteral("PWM_FREQUENCY"), 48);
  QVERIFY(esc.isValid());
}

QTEST_MAIN(ModelTypesTests)
#include "test_models_types.moc"
