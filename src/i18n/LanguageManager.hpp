#pragma once

#include <QObject>
#include <QString>
#include <QVector>

class QTranslator;

class LanguageManager final : public QObject {
  Q_OBJECT

 public:
  struct LanguageOption {
    QString code;
    QString label;
  };

  static LanguageManager& instance();

  QVector<LanguageOption> availableLanguages() const;
  QString currentLanguage() const;
  bool setLanguage(const QString& languageCode);

 signals:
  void languageChanged(const QString& languageCode);

 private:
  explicit LanguageManager(QObject* parent = nullptr);

  QVector<LanguageOption> loadAvailableLanguages() const;
  static QString normalizeLanguageCode(const QString& languageCode);

  QVector<LanguageOption> availableLanguages_;
  QString currentLanguage_{QStringLiteral("en")};
  QTranslator* translator_{nullptr};
};
