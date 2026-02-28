#include "AppSettingsDialog.hpp"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFile>
#include <QFormLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QRegularExpression>
#include <QScrollArea>
#include <QSettings>
#include <QVBoxLayout>

AppSettingsDialog::AppSettingsDialog(QWidget* parent) : QDialog(parent) {
  setObjectName(QStringLiteral("appSettingsDialog"));
  setWindowTitle(tr("Application Settings"));
  resize(520, 430);

  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(12, 12, 12, 12);
  root->setSpacing(10);

  auto* title = new QLabel(tr("Configure application behavior"), this);
  title->setObjectName(QStringLiteral("homeSectionTitle"));

  auto* scroll = new QScrollArea(this);
  scroll->setWidgetResizable(true);

  auto* settingsContainer = new QWidget(scroll);
  auto* settingsLayout = new QFormLayout(settingsContainer);
  settingsLayout->setLabelAlignment(Qt::AlignLeft);
  settingsLayout->setFormAlignment(Qt::AlignTop | Qt::AlignLeft);
  settingsLayout->setHorizontalSpacing(16);
  settingsLayout->setVerticalSpacing(10);

  loadDefaults();
  for (auto& setting : settings_) {
    setting.checkBox = new QCheckBox(settingsContainer);
    setting.checkBox->setChecked(setting.defaultValue);
    settingsLayout->addRow(setting.label, setting.checkBox);
  }

  scroll->setWidget(settingsContainer);
  loadPersistedValues();

  auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &AppSettingsDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  root->addWidget(title);
  root->addWidget(scroll, 1);
  root->addWidget(buttonBox);
}

bool AppSettingsDialog::settingValue(const QString& key) const {
  for (const auto& setting : settings_) {
    if (setting.key == key && setting.checkBox != nullptr) {
      return setting.checkBox->isChecked();
    }
  }

  return false;
}

void AppSettingsDialog::accept() {
  savePersistedValues();
  QDialog::accept();
}

QString AppSettingsDialog::prettifyKey(const QString& key) {
  QString normalized = key;
  normalized.replace(QRegularExpression(QStringLiteral("([a-z0-9])([A-Z])")), QStringLiteral("\\1 \\2"));
  if (!normalized.isEmpty()) {
    normalized[0] = normalized[0].toUpper();
  }

  return normalized;
}

void AppSettingsDialog::loadDefaults() {
  settings_.clear();

  QFile file(QStringLiteral(":/data/settings.json"));
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return;
  }

  const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  if (!doc.isObject()) {
    return;
  }

  const QJsonObject root = doc.object();
  const QJsonObject defaults = root.value(QStringLiteral("defaultAppSettings")).toObject();
  if (defaults.isEmpty()) {
    return;
  }

  for (auto it = defaults.constBegin(); it != defaults.constEnd(); ++it) {
    const QJsonObject settingObject = it.value().toObject();
    const QString type = settingObject.value(QStringLiteral("type")).toString();
    if (type != QStringLiteral("boolean")) {
      continue;
    }

    settings_.push_back(BooleanSetting{it.key(), prettifyKey(it.key()), settingObject.value(QStringLiteral("value")).toBool(false), nullptr});
  }
}

void AppSettingsDialog::loadPersistedValues() {
  QSettings appSettings;
  for (auto& setting : settings_) {
    if (setting.checkBox == nullptr) {
      continue;
    }

    const QVariant persisted =
        appSettings.value(QStringLiteral("appSettings/%1").arg(setting.key), setting.defaultValue);
    setting.checkBox->setChecked(persisted.toBool());
  }
}

void AppSettingsDialog::savePersistedValues() const {
  QSettings appSettings;
  for (const auto& setting : settings_) {
    if (setting.checkBox == nullptr) {
      continue;
    }

    appSettings.setValue(QStringLiteral("appSettings/%1").arg(setting.key), setting.checkBox->isChecked());
  }
}
