#include <QObject>
#include <QSignalSpy>
#include <QtTest>

#include "models/EscListModel.hpp"

class EscListModelTests final : public QObject {
  Q_OBJECT

 private slots:
  void exposesEscDataByRoles();
  void setFromVariantList_skipsInvalidEntries();
  void updateEsc_emitsDataChanged();
};

void EscListModelTests::exposesEscDataByRoles() {
  models::EscListModel model;

  models::EscConfig esc;
  esc.index = 0;
  esc.name = QStringLiteral("ESC 1");
  esc.firmwareName = QStringLiteral("Bluejay");
  esc.firmwareVersion = QStringLiteral("0.22.2");
  esc.layout = QStringLiteral("A_H_20");
  esc.selected = true;
  esc.settings = {{QStringLiteral("PWM_FREQUENCY"), 48}};

  model.setEscs({esc});

  QCOMPARE(model.rowCount(), 1);
  const QModelIndex idx = model.index(0, 0);
  QCOMPARE(model.data(idx, models::EscListModel::NameRole).toString(), QStringLiteral("ESC 1"));
  QCOMPARE(model.data(idx, models::EscListModel::FirmwareNameRole).toString(), QStringLiteral("Bluejay"));
  QCOMPARE(model.data(idx, models::EscListModel::SettingsRole).toMap().value(QStringLiteral("PWM_FREQUENCY")).toInt(), 48);
}

void EscListModelTests::setFromVariantList_skipsInvalidEntries() {
  models::EscListModel model;

  const QVariantList input = {
      QVariantMap{{QStringLiteral("index"), 0},
                  {QStringLiteral("name"), QStringLiteral("ESC 1")},
                  {QStringLiteral("firmwareName"), QStringLiteral("Bluejay")},
                  {QStringLiteral("firmwareVersion"), QStringLiteral("0.22.2")},
                  {QStringLiteral("layout"), QStringLiteral("A_H_20")}},
      QVariantMap{{QStringLiteral("index"), 1},
                  {QStringLiteral("name"), QStringLiteral("ESC 2")},
                  {QStringLiteral("firmwareName"), QStringLiteral("")},
                  {QStringLiteral("firmwareVersion"), QStringLiteral("16.7")},
                  {QStringLiteral("layout"), QStringLiteral("S_H_30")}},
  };

  model.setFromVariantList(input);

  QCOMPARE(model.rowCount(), 1);
  QCOMPARE(model.toVariantList().size(), 1);
  QCOMPARE(model.data(model.index(0, 0), models::EscListModel::NameRole).toString(), QStringLiteral("ESC 1"));
}

void EscListModelTests::updateEsc_emitsDataChanged() {
  models::EscListModel model;

  models::EscConfig esc;
  esc.index = 0;
  esc.name = QStringLiteral("ESC 1");
  esc.firmwareName = QStringLiteral("Bluejay");
  esc.firmwareVersion = QStringLiteral("0.22.2");

  model.setEscs({esc});

  QSignalSpy spy(&model, &models::EscListModel::dataChanged);

  models::EscConfig updated = esc;
  updated.name = QStringLiteral("ESC 1 Updated");
  QVERIFY(model.updateEsc(0, updated));
  QCOMPARE(spy.count(), 1);
  QCOMPARE(model.data(model.index(0, 0), models::EscListModel::NameRole).toString(),
           QStringLiteral("ESC 1 Updated"));

  updated.firmwareName.clear();
  QVERIFY(!model.updateEsc(0, updated));
}

QTEST_MAIN(EscListModelTests)
#include "test_esc_list_model.moc"
