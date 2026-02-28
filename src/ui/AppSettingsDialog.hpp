#pragma once

#include <QDialog>

#include <QVector>

class QCheckBox;

class AppSettingsDialog final : public QDialog {
  Q_OBJECT

 public:
  explicit AppSettingsDialog(QWidget* parent = nullptr);

  bool settingValue(const QString& key) const;

 private slots:
  void accept() override;

 private:
  struct BooleanSetting {
    QString key;
    QString label;
    bool defaultValue{false};
    QCheckBox* checkBox{nullptr};
  };

  static QString prettifyKey(const QString& key);

  void loadDefaults();
  void loadPersistedValues();
  void savePersistedValues() const;

  QVector<BooleanSetting> settings_;
};
