#pragma once

#include <QDialog>

class QCheckBox;
class QComboBox;

class FirmwareSelectorDialog final : public QDialog {
  Q_OBJECT

 public:
  explicit FirmwareSelectorDialog(QWidget* parent = nullptr);

  QString selectedSource() const;
  QString selectedVersion() const;
  QString selectedEscVariant() const;
  bool forceFlashEnabled() const;
  bool migrateEnabled() const;

 private slots:
  void onSourceChanged();

 private:
  void rebuildVersionOptions();
  void rebuildEscVariantOptions();

  QComboBox* sourceCombo_;
  QComboBox* versionCombo_;
  QComboBox* escVariantCombo_;
  QCheckBox* forceFlashCheck_;
  QCheckBox* migrateCheck_;
};
