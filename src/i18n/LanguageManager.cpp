#include "LanguageManager.hpp"

#include <QApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QSettings>
#include <QTranslator>

LanguageManager& LanguageManager::instance() {
  static LanguageManager manager;
  return manager;
}

QVector<LanguageManager::LanguageOption> LanguageManager::availableLanguages() const {
  return availableLanguages_;
}

QString LanguageManager::currentLanguage() const { return currentLanguage_; }

bool LanguageManager::setLanguage(const QString& languageCode) {
  const QString normalized = normalizeLanguageCode(languageCode);
  if (normalized.isEmpty()) {
    return false;
  }

  if (normalized == currentLanguage_) {
    return true;
  }

  bool supported = false;
  for (const auto& option : availableLanguages_) {
    if (option.code == normalized) {
      supported = true;
      break;
    }
  }

  if (!supported) {
    return false;
  }

  if (translator_ != nullptr) {
    qApp->removeTranslator(translator_);
    delete translator_;
    translator_ = nullptr;
  }

  if (normalized != QStringLiteral("en")) {
    auto* newTranslator = new QTranslator(this);
    const QString qmFile = QStringLiteral(":/i18n/escc_%1.qm").arg(normalized);
    if (!newTranslator->load(qmFile)) {
      delete newTranslator;
      return false;
    }

    qApp->installTranslator(newTranslator);
    translator_ = newTranslator;
  }

  currentLanguage_ = normalized;
  QSettings settings;
  settings.setValue(QStringLiteral("i18n/language"), currentLanguage_);

  emit languageChanged(currentLanguage_);
  return true;
}

LanguageManager::LanguageManager(QObject* parent)
    : QObject(parent), availableLanguages_(loadAvailableLanguages()) {
  QSettings settings;
  const QString persisted = settings.value(QStringLiteral("i18n/language"), QStringLiteral("en")).toString();
  currentLanguage_ = QStringLiteral("en");
  if (!persisted.isEmpty()) {
    const bool switched = setLanguage(persisted);
    if (!switched) {
      currentLanguage_ = QStringLiteral("en");
    }
  }
}

QVector<LanguageManager::LanguageOption> LanguageManager::loadAvailableLanguages() const {
  QVector<LanguageOption> options;

  QFile file(QStringLiteral(":/data/settings.json"));
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return {{QStringLiteral("en"), QStringLiteral("English")}};
  }

  const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  if (!doc.isObject()) {
    return {{QStringLiteral("en"), QStringLiteral("English")}};
  }

  const QJsonArray langs = doc.object().value(QStringLiteral("availableLanguages")).toArray();
  for (const auto& item : langs) {
    if (!item.isObject()) {
      continue;
    }

    const QJsonObject obj = item.toObject();
    const QString code = normalizeLanguageCode(obj.value(QStringLiteral("value")).toString());
    const QString label = obj.value(QStringLiteral("label")).toString();
    if (code.isEmpty()) {
      continue;
    }

    options.push_back({code, label.isEmpty() ? code : label});
  }

  if (options.isEmpty()) {
    options.push_back({QStringLiteral("en"), QStringLiteral("English")});
  }

  return options;
}

QString LanguageManager::normalizeLanguageCode(const QString& languageCode) {
  QString code = languageCode.trimmed().toLower();
  if (code.isEmpty()) {
    return {};
  }

  code.replace(QLatin1Char('-'), QLatin1Char('_'));
  return code;
}
