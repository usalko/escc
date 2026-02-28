#include "FirmwareSelectorDialog.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

FirmwareSelectorDialog::FirmwareSelectorDialog(QWidget* parent)
    : QDialog(parent),
      sourceCombo_(new QComboBox(this)),
      versionCombo_(new QComboBox(this)),
      escVariantCombo_(new QComboBox(this)),
      forceFlashCheck_(new QCheckBox(tr("Force flash"), this)),
      migrateCheck_(new QCheckBox(tr("Migrate settings"), this)) {
  setObjectName(QStringLiteral("firmwareSelectorDialog"));
  setWindowTitle(tr("Select Firmware"));
  setModal(true);
  resize(480, 280);

  auto* root = new QVBoxLayout(this);
  auto* note = new QLabel(
      tr("Choose firmware source, version and ESC variant before flashing."), this);
  note->setWordWrap(true);

  auto* form = new QFormLayout();
  form->addRow(tr("Source"), sourceCombo_);
  form->addRow(tr("Version"), versionCombo_);
  form->addRow(tr("ESC Variant"), escVariantCombo_);

  sourceCombo_->addItem(QStringLiteral("BLHeli_S"));
  sourceCombo_->addItem(QStringLiteral("Bluejay"));
  sourceCombo_->addItem(QStringLiteral("AM32"));

  forceFlashCheck_->setChecked(false);
  migrateCheck_->setChecked(false);

  auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Select"));

  root->addWidget(note);
  root->addLayout(form);
  root->addWidget(forceFlashCheck_);
  root->addWidget(migrateCheck_);
  root->addStretch(1);
  root->addWidget(buttonBox);

  connect(sourceCombo_, &QComboBox::currentIndexChanged, this, &FirmwareSelectorDialog::onSourceChanged);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  onSourceChanged();
}

QString FirmwareSelectorDialog::selectedSource() const {
  return sourceCombo_->currentText();
}

QString FirmwareSelectorDialog::selectedVersion() const {
  return versionCombo_->currentText();
}

QString FirmwareSelectorDialog::selectedEscVariant() const {
  return escVariantCombo_->currentText();
}

bool FirmwareSelectorDialog::forceFlashEnabled() const {
  return forceFlashCheck_->isChecked();
}

bool FirmwareSelectorDialog::migrateEnabled() const {
  return migrateCheck_->isChecked();
}

void FirmwareSelectorDialog::onSourceChanged() {
  rebuildVersionOptions();
  rebuildEscVariantOptions();
}

void FirmwareSelectorDialog::rebuildVersionOptions() {
  versionCombo_->clear();

  const QString source = selectedSource();
  if (source == QStringLiteral("BLHeli_S")) {
    versionCombo_->addItems({QStringLiteral("16.7"), QStringLiteral("16.6")});
    return;
  }

  if (source == QStringLiteral("Bluejay")) {
    versionCombo_->addItems({QStringLiteral("0.22.2"), QStringLiteral("0.21.0")});
    return;
  }

  versionCombo_->addItems({QStringLiteral("1.99"), QStringLiteral("1.98")});
}

void FirmwareSelectorDialog::rebuildEscVariantOptions() {
  escVariantCombo_->clear();

  const QString source = selectedSource();
  if (source == QStringLiteral("BLHeli_S")) {
    escVariantCombo_->addItems(
        {QStringLiteral("S_H_50"), QStringLiteral("A_H_20"), QStringLiteral("HGLRC_FORWARD")});
    return;
  }

  if (source == QStringLiteral("Bluejay")) {
    escVariantCombo_->addItems({QStringLiteral("A_X_5"), QStringLiteral("A_H_20"), QStringLiteral("S_H_30")});
    return;
  }

  escVariantCombo_->addItems({QStringLiteral("F051_48K"), QStringLiteral("F421_64K")});
}
